
// Read-only protocol copied from the supplied C++ WorkThread.cpp.
const QUERIES = [
  {key: 'temperature', address: '2010', command: '?:2010:00::a6\r'},
  {key: 'co2', address: '2020', command: '?:2020:00::c0\r'},
  {key: 'water', address: '204a', command: '?:204a:00::cd\r'},
];

function parseReply(line, expectedAddress) {
  const clean = line.replace(/^[\x00\s]+|[\x00\s]+$/g, '');
  const match = /^!:([0-9a-f]{4}):([0-9a-f]{2}):([^:]*):([0-9a-f]{2})$/i.exec(clean);
  if (!match) throw new Error('Malformed serial reply');
  const [, address, size, payload, checksum] = match;
  if (address.toLowerCase() !== expectedAddress) throw new Error('Unexpected reply address');
  if (parseInt(size, 16) !== payload.length) throw new Error('Reply length mismatch');
  const fields = payload.split(';');
  const minimum = expectedAddress === '204a' ? 1 : 2;
  if (fields.length < minimum || fields.some(v => !/^[+-]?\d+(?:\.\d+)?$/.test(v))) {
    throw new Error('Invalid measurement payload');
  }
  const values = fields.map(Number);
  if (!values.every(Number.isFinite)) throw new Error('Non-finite measurement');
  // Legacy code uses field 2 for measured T/CO2 and field 1 for water.
  // BCC algorithm was not included: preserve it for diagnostics, do not claim verification.
  return {value: values[minimum - 1], setpoint: minimum === 2 ? values[0] : null, checksum, raw: clean};
}

class LineFramer {
  constructor() { this.buffer = ''; }
  push(chunk) {
    this.buffer += chunk;
    if (this.buffer.length > 8192) { this.buffer = ''; throw new Error('Serial input overflow'); }
    const pieces = this.buffer.split(/[\r\n]+/);
    this.buffer = pieces.pop();
    return pieces.map(s => s.replace(/\x00/g, '').trim()).filter(Boolean);
  }
}

const DEFAULTS = {
  pollSeconds: 30, saveMinutes: 5, retentionDays: 60,
  delays: {temperatureLow: 15, temperatureHigh: 5, co2Low: 5, co2High: 5, water: 5, connection: 5},
  incubators: [{id: '1', name: 'Incubator 1', port: '', baud: 57600,
    thresholds: {temperatureLow: 36, temperatureHigh: 37.5, co2Low: 4, co2High: 5, waterLow: 0.01}}],
};
const ALARM_LABELS = {
  temperatureLow: 'Temperature low', temperatureHigh: 'Temperature high',
  co2Low: 'CO₂ low', co2High: 'CO₂ high', water: 'Water level low', connection: 'Incomplete or lost connection',
};

function validateConfig(config) {
  if (!config || !Array.isArray(config.incubators) || config.incubators.length < 1 || config.incubators.length > 9) {
    throw new Error('Configure between 1 and 9 incubators.');
  }
  const range = (v, lo, hi, label) => {
    if (typeof v !== 'number' || !Number.isFinite(v) || v < lo || v > hi) throw new Error(`Invalid ${label}.`);
  };
  range(config.pollSeconds, 1, 3600, 'poll interval');
  range(config.saveMinutes, 0.01, 1440, 'archive interval');
  range(config.retentionDays, 1, 3650, 'retention period');
  for (const key of Object.keys(ALARM_LABELS)) range(config.delays?.[key], 0, 1440, `${key} delay`);
  const ids = new Set();
  const ports = new Set();
  for (const inc of config.incubators) {
    if (typeof inc.id !== 'string' || !/^[a-zA-Z0-9_-]{1,32}$/.test(inc.id) || ids.has(inc.id)) throw new Error('Incubator IDs must be unique.');
    ids.add(inc.id);
    if (typeof inc.name !== 'string' || !inc.name.trim() || inc.name.length > 100) throw new Error('Each incubator needs a name.');
    if (typeof inc.port !== 'string' || inc.port.length > 150) throw new Error('Invalid serial port.');
    if (inc.port && ports.has(inc.port)) throw new Error('Each incubator needs a different serial port.');
    if (inc.port) ports.add(inc.port);
    range(inc.baud, 300, 1000000, 'baud rate');
    if (!Number.isInteger(inc.baud)) throw new Error('Baud rate must be an integer.');
    for (const key of Object.keys(DEFAULTS.incubators[0].thresholds)) range(inc.thresholds?.[key], 0, 1000, `${key} threshold`);
    for (const sensor of ['temperature', 'co2']) {
      if (inc.thresholds[sensor + 'Low'] >= inc.thresholds[sensor + 'High']) throw new Error(`${sensor}: minimum must be below maximum.`);
    }
  }
  return config;
}

class AlarmEngine {
  constructor(config, inc, onEvent = () => {}) {
    this.config = config; this.inc = inc; this.onEvent = onEvent; this.states = {};
  }
  evaluate(sample, now = Date.now()) {
    const t = this.inc.thresholds;
    const v = sample.values || {};
    const valid = k => typeof v[k] === 'number' && Number.isFinite(v[k]);
    // Any missing channel is a connection fault, never a successful poll.
    const conditions = {
      temperatureLow: valid('temperature') ? v.temperature < t.temperatureLow : null,
      temperatureHigh: valid('temperature') ? v.temperature > t.temperatureHigh : null,
      co2Low: valid('co2') ? v.co2 < t.co2Low : null,
      co2High: valid('co2') ? v.co2 > t.co2High : null,
      water: valid('water') ? v.water < t.waterLow : null,
      connection: !['temperature', 'co2', 'water'].every(valid),
    };
    for (const [key, condition] of Object.entries(conditions)) {
      const old = this.states[key];
      if (condition === null) { // interrupted evidence: keep active alarms, reset pending timers
        if (old?.status === 'pending') delete this.states[key];
        continue;
      }
      if (!condition) {
        if (old?.status === 'active') this.onEvent({key, type: 'recovery', at: now, incubatorId: this.inc.id, message: `${this.inc.name}: ${ALARM_LABELS[key]} recovered`});
        delete this.states[key];
        continue;
      }
      const state = old || {since: now, status: 'pending'};
      this.states[key] = state;
      if (state.status !== 'active' && now - state.since >= this.config.delays[key] * 60000) {
        state.status = 'active';
        this.onEvent({key, type: 'alarm', at: now, incubatorId: this.inc.id, message: `${this.inc.name}: ${ALARM_LABELS[key]}`});
      }
    }
    return structuredClone(this.states);
  }
}

function csvText(rows, incubators) {
  const cell = v => {
    let text = v == null ? '' : String(v);
    if (/^[=+@-]/.test(text) && typeof v !== 'number') text = "'" + text;
    return '"' + text.replaceAll('"', '""') + '"';
  };
  const names = Object.fromEntries(incubators.map(i => [i.id, i.name]));
  return ['timestamp_utc,incubator_id,incubator_name,temperature_c,co2_percent,water_raw,complete,errors',
    ...rows.map(r => [new Date(r.at).toISOString(), r.incubatorId, names[r.incubatorId] || r.incubatorId,
      r.values.temperature, r.values.co2, r.values.water,
      ['temperature','co2','water'].every(k => Number.isFinite(r.values[k]) && r.values[k] != null),
      Object.values(r.errors || {}).join(' | ')].map(cell).join(','))].join('\r\n');
}


class SerialClient {
  constructor(port, onDiagnostic = () => {}) {
    this.port = port; this.onDiagnostic = onDiagnostic; this.pending = null; this.closing = false;
    this.framer = new LineFramer();
  }
  async open(baud) {
    await this.port.open({baudRate: baud, dataBits: 8, stopBits: 1, parity: 'none', flowControl: 'none'});
    this.writer = this.port.writable.getWriter();
    this.readTask = this.readLoop();
  }
  async readLoop() {
    try {
      while (!this.closing && this.port.readable) {
        this.reader = this.port.readable.getReader();
        try {
          while (!this.closing) {
            const {value, done} = await this.reader.read();
            if (done) break;
            for (const line of this.framer.push(new TextDecoder().decode(value))) {
              this.onDiagnostic('RX ' + line);
              if (this.pending && line.toLowerCase().startsWith(`!:${this.pending.address}:`)) {
                try { this.pending.resolve(parseReply(line, this.pending.address)); }
                catch (error) { this.pending.reject(error); }
              }
            }
          }
        } finally { this.reader.releaseLock(); this.reader = null; }
        if (!this.closing) throw new Error('Serial stream ended. Reconnect this incubator.');
      }
    } catch (error) {
      this.failure = error;
      this.pending?.reject(error);
    }
  }
  query(query, timeoutMs = 3000) {
    if (this.failure) return Promise.reject(this.failure);
    if (this.closing || !this.writer) return Promise.reject(new Error('Serial port is closed'));
    if (this.pending) return Promise.reject(new Error('Serial request already in progress'));
    this.framer.buffer = '';
    return new Promise((resolve, reject) => {
      let settled = false;
      const finish = (fn, value) => { if (settled) return; settled = true; clearTimeout(timer); this.pending = null; fn(value); };
      const timer = setTimeout(() => finish(reject, new Error(`Timeout reading ${query.key}`)), timeoutMs);
      this.pending = {address: query.address, resolve: v => finish(resolve, v), reject: e => finish(reject, e)};
      const request = this.pending;
      this.onDiagnostic('TX ' + query.command.trim());
      this.writer.write(new TextEncoder().encode(query.command)).catch(e => request.reject(e));
    });
  }
  async poll() {
    const sample = {values: {}, errors: {}, raw: {}};
    for (const query of QUERIES) {
      try {
        const reply = await this.query(query);
        sample.values[query.key] = reply.value;
        sample.raw[query.key] = reply.raw;
      } catch (error) {
        sample.values[query.key] = null; sample.errors[query.key] = error.message;
      }
    }
    return sample;
  }
  async close() {
    this.closing = true;
    this.pending?.reject(new Error('Monitoring stopped'));
    await this.reader?.cancel().catch(() => {});
    await this.readTask;
    if (this.writer) { this.writer.releaseLock(); this.writer = null; }
    await this.port.close().catch(() => {});
  }
}

class Archive {
  async open() {
    this.db = await new Promise((resolve, reject) => {
      const req = indexedDB.open('incubator-monitor-v1', 1);
      req.onupgradeneeded = () => {
        const store = req.result.createObjectStore('samples', {keyPath: 'key', autoIncrement: true});
        store.createIndex('at', 'at');
      };
      req.onsuccess = () => resolve(req.result); req.onerror = () => reject(req.error);
    });
  }
  add(row) {
    return new Promise((resolve, reject) => {
      const tx = this.db.transaction('samples', 'readwrite');
      tx.objectStore('samples').add(row);
      tx.oncomplete = resolve; tx.onerror = () => reject(tx.error);
    });
  }
  rows(since, until = Date.now()) {
    return new Promise((resolve, reject) => {
      const req = this.db.transaction('samples').objectStore('samples').index('at').getAll(IDBKeyRange.bound(since, until));
      req.onsuccess = () => resolve(req.result); req.onerror = () => reject(req.error);
    });
  }
  prune(before) {
    return new Promise((resolve, reject) => {
      const tx = this.db.transaction('samples', 'readwrite');
      const req = tx.objectStore('samples').index('at').openCursor(IDBKeyRange.upperBound(before, true));
      req.onsuccess = () => { const cursor = req.result; if (cursor) {cursor.delete(); cursor.continue();} };
      tx.oncomplete = resolve; tx.onerror = () => reject(tx.error);
    });
  }
}


const $ = id => document.getElementById(id);
const escapeHtml = s => String(s ?? '').replace(/[&<>"']/g, ch => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[ch]));
const clone = object => structuredClone(object);
const today = () => { const d = new Date(); d.setMinutes(d.getMinutes() - d.getTimezoneOffset()); return d.toISOString().slice(0,10); };
let savedConfig;
try { savedConfig = validateConfig(JSON.parse(localStorage.getItem('incubator-config-v1'))); } catch { savedConfig = clone(DEFAULTS); }
let config = savedConfig, mode = 'demo', running = false, busy = false, service = null, token = '', latest = {}, engines = new Map(), clients = new Map();
let timers = new Map(), lastSave = new Map(), archive = new Archive(), archiveReady = false, events = [], chartRows = [], demoRows = [], draft, demoStarted;
let audio, notifications = false, storageError = '', bridgeError = '', chartBusy = false, bridgeBusy = false, session = 0;
try { events = JSON.parse(localStorage.getItem('incubator-events-v1')) || []; } catch {}
let realEvents = clone(events);

function toast(message) { $('toast').textContent = message; $('toast').hidden = false; clearTimeout(toast.timer); toast.timer = setTimeout(() => $('toast').hidden = true, 6000); }
function download(name, text, type) { const url = URL.createObjectURL(new Blob([text], {type})); const a = document.createElement('a'); a.href = url; a.download = name; a.click(); setTimeout(() => URL.revokeObjectURL(url), 5000); }
async function api(path, body) {
  const response = await fetch('/api/' + path, body === undefined ? {cache:'no-store', signal:AbortSignal.timeout(15000)} : {
    method: 'POST', headers: {'Content-Type':'application/json','X-Monitor-Token': token}, body: JSON.stringify(body), signal:AbortSignal.timeout(15000)});
  if (!response.ok) { let result; try {result = await response.json();} catch {} throw new Error(result?.error || 'Background service unavailable'); }
  return response.json();
}
function eventReceived(event) {
  events.unshift(event); events = events.slice(0,100);
  if (mode !== 'demo') { realEvents = clone(events); try { localStorage.setItem('incubator-events-v1', JSON.stringify(events)); } catch { storageError = 'Alarm log could not be saved in this browser.'; } }
  if (event.type === 'alarm' && notifications) {
    beep();
    if ('Notification' in window && Notification.permission === 'granted') new Notification('Incubator alarm', {body: event.message});
  }
}
function beep() {
  if (!audio || audio.state !== 'running') return;
  const osc = audio.createOscillator(), gain = audio.createGain();
  osc.frequency.value = 660; gain.gain.setValueAtTime(0.12, audio.currentTime); gain.gain.exponentialRampToValueAtTime(0.001, audio.currentTime + 0.6);
  osc.connect(gain); gain.connect(audio.destination); osc.start(); osc.stop(audio.currentTime + 0.6);
}
function demoConfig() {
  const c = clone(DEFAULTS); c.pollSeconds = 2;
  c.incubators = ['Primary culture','Retinal explants','Reserve incubator'].map((name,index) => ({...clone(DEFAULTS.incubators[0]), id:String(index+1), name}));
  c.delays.water = 0; return c;
}
function makeDemo() {
  const now = Date.now(); demoStarted = now;
  engines = new Map(config.incubators.map(i => [i.id, new AlarmEngine(config, i, eventReceived)]));
  demoRows = [];
  for (let minutes = 180; minutes >= 0; minutes -= 3) {
    for (const inc of config.incubators) {
      const phase = minutes / 17 + Number(inc.id);
      demoRows.push({at:now - minutes * 60000, incubatorId: inc.id,
        values:{temperature:37 + Math.sin(phase) * 0.07, co2:4.8 + Math.cos(phase) * 0.06, water: inc.id === '3' ? 0 : 1}, errors:{}});
    }
  }
}
function render() {
  const now = Date.now(), isDemo = mode === 'demo' || service?.demo && mode === 'bridge';
  $('mode').value = mode;
  $('run').textContent = running ? 'Stop monitoring' : 'Start monitoring'; $('run').disabled = busy;
  $('run-label').textContent = running ? isDemo ? 'Demo running' : 'Monitoring' : 'Stopped';
  $('run-dot').style.background = running ? '#167460' : '#a6b2aa';
  $('unit-count').textContent = `${config.incubators.length} incubator${config.incubators.length > 1 ? 's' : ''}`;
  $('archive-label').textContent = `Reads: ${config.pollSeconds}s · Archive: ${config.saveMinutes} min`;
  let note = isDemo ? 'DEMO · Simulated readings. Reserve incubator has a simulated low-water alarm. No hardware is connected and no email is sent.' :
    mode === 'bridge' ? 'Background service · Monitoring and email alarms continue while Chrome is closed. Keep the service process and computer running.' :
    'Chrome serial · Connect each incubator, then start monitoring. Keep this tab open and visible and the computer awake. For continuous monitoring and email, use the background service.';
  if (bridgeError && mode === 'bridge') note = bridgeError + ' · The dashboard cannot verify monitoring status.';
  if (storageError && mode === 'direct') note += ' ' + storageError;
  if (mode === 'direct' && !('serial' in navigator)) note += ' Web Serial is unavailable here. Open in desktop Chrome at localhost or HTTPS.';
  if (mode === 'direct' && document.hidden && running) note += ' This tab is hidden; sampling may be delayed.';
  $('notice').textContent = note; $('notice').className = 'notice' + (bridgeError && mode === 'bridge' || storageError && mode === 'direct' ? ' error' : mode === 'direct' ? ' warn' : '');
  $('cards').innerHTML = config.incubators.map(inc => {
    const sample = latest[inc.id], values = sample?.values || {}, alarms = Object.entries(sample?.alarms || {});
    const stale = running && sample && now - sample.at > Math.max(15000, config.pollSeconds * 2000 + 10000);
    const failures = Object.values(sample?.errors || {});
    const active = alarms.filter(([,s]) => s.status === 'active'), pending = alarms.filter(([,s]) => s.status === 'pending');
    const bad = active.length || stale || bridgeError && mode === 'bridge';
    let label = !running ? clients.has(inc.id) ? 'READY' : 'STOPPED' : !sample ? 'WAITING' : bad ? 'ALARM' : failures.length ? 'READ ERROR' : pending.length ? 'PENDING' : 'IN RANGE';
    const badge = !running ? '' : bad || failures.length ? 'bad' : pending.length ? 'pending' : sample ? 'good' : '';
    const show = key => typeof values[key] === 'number' && Number.isFinite(values[key]) ? values[key].toFixed(2) : '—';
    const metricBad = prefix => alarms.some(([key]) => key.startsWith(prefix));
    const water = values.water == null ? 'Unknown' : values.water < inc.thresholds.waterLow ? 'Low' : 'OK';
    const age = sample ? `Last read ${new Date(sample.at).toLocaleTimeString([], {hour:'2-digit',minute:'2-digit',second:'2-digit'})}` : 'No readings yet';
    const faults = [...active.map(([k])=>ALARM_LABELS[k]), ...pending.map(([k,s])=>`${ALARM_LABELS[k]} · pending ${Math.max(0, Math.ceil(config.delays[k] - (now-s.since)/60000))} min`), ...failures];
    if (stale) faults.push('Readings are stale; verify the monitor service and connection.');
    return `<article class="card ${bad?'alarm':pending.length?'pending':''}"><div class="card-heading"><div><div class="unit-name">${escapeHtml(inc.name)}</div><div class="unit-subtitle">UNIT ${escapeHtml(inc.id)}${isDemo?' · SIMULATED':''}</div></div><span class="badge ${badge}">${label}</span></div>
      <div class="metric"><span class="metric-label">Temperature</span><div><div class="metric-value ${metricBad('temperature')?'bad':''}">${show('temperature')}<small>°C</small></div><div class="metric-range">${inc.thresholds.temperatureLow}–${inc.thresholds.temperatureHigh} °C</div></div></div>
      <div class="metric"><span class="metric-label">CO₂</span><div><div class="metric-value ${metricBad('co2')?'bad':''}">${show('co2')}<small>%</small></div><div class="metric-range">${inc.thresholds.co2Low}–${inc.thresholds.co2High} %</div></div></div>
      <div class="metric"><span class="metric-label">Water level</span><span class="metric-value water ${metricBad('water')?'bad':''}">${water}</span></div>
      <div class="card-bottom"><span>${age}${!running&&sample?' · stopped':''}</span>${mode==='direct'?`<button data-connect="${escapeHtml(inc.id)}" ${running?'disabled':''}>${clients.has(inc.id)?'Disconnect':'Connect serial'}</button>`:'<span>8N1 · '+inc.baud+' baud</span>'}</div>${faults.length?'<div class="card-fault">'+escapeHtml([...new Set(faults)].join(' · '))+'</div>':''}</article>`;
  }).join('');
  $('events').innerHTML = events.length ? events.slice(0,12).map(e => `<div class="event-row"><div><span class="event-kind ${e.type}">${e.type==='alarm'?'ALARM':'RECOVERED'}</span>${escapeHtml(e.message)}</div><time>${escapeHtml(new Date(e.at).toLocaleString())}</time></div>`).join('') : '<div class="empty">No alarms recorded. New alarms and recoveries will appear here.</div>';
  $('settings-open').disabled = running || clients.size > 0;
  $('email-status').textContent = mode === 'bridge' && service ? service.demo ? 'Demo · email disabled' : !service.email.enabled ? 'Email not configured' : service.email.lastError ? `Email delivery failed · ${service.email.pending} queued` : `Email enabled · ${service.email.pending} queued` : 'Browser alerts only';
  $('footer-note').textContent = isDemo ? 'Demonstration · Simulated data only' : 'Local monitoring · No cloud account required';
  $('clock').textContent = new Date().toLocaleString([], {weekday:'short',month:'short',day:'numeric',hour:'2-digit',minute:'2-digit'});
}

function updateChartUnits() {
  const selected = $('chart-unit').value;
  $('chart-unit').innerHTML = config.incubators.map(i => `<option value="${escapeHtml(i.id)}">${escapeHtml(i.name)}</option>`).join('');
  if (config.incubators.some(i=>i.id===selected)) $('chart-unit').value=selected;
}
function drawChart(rows, key, low, high, color) {
  const width=500, height=185, left=44, right=10, top=18, bottom=31;
  const good = rows.filter(r=> typeof r.values[key] === 'number' && Number.isFinite(r.values[key]));
  let min = Math.min(low, ...good.map(r=>r.values[key])), max = Math.max(high, ...good.map(r=>r.values[key]));
  const pad=Math.max((max-min)*.18,.15); min-=pad; max+=pad;
  const start=rows.length ? rows[0].at : Date.now()-3600000, end=rows.length>1 ? rows.at(-1).at : start+3600000;
  const x=at=>left+(at-start)/(end-start||1)*(width-left-right), y=v=>height-bottom-(v-min)/(max-min)*(height-top-bottom);
  let svg=`<svg viewBox="0 0 ${width} ${height}" role="img" aria-label="${key==='temperature'?'Temperature in Celsius':'CO2 in percent'} trend"><rect x="${left}" y="${y(high)}" width="${width-left-right}" height="${y(low)-y(high)}" fill="${color}" opacity=".055"/>`;
  for(let i=0;i<4;i++) {const v=min+(max-min)*i/3; svg+=`<line x1="${left}" y1="${y(v)}" x2="${width-right}" y2="${y(v)}" stroke="#e9eeea"/><text x="${left-8}" y="${y(v)+3}" font-size="10" text-anchor="end" fill="#91a092">${v.toFixed(1)}</text>`;}
  for(let i=0;i<4;i++){const at=start+(end-start)*i/3;svg+=`<text x="${x(at)}" y="${height-7}" text-anchor="middle" font-size="10" fill="#91a092">${new Date(at).toLocaleTimeString([],{hour:'2-digit',minute:'2-digit'})}</text>`;}
  const maxGap=Math.max(config.saveMinutes*60*1000,config.pollSeconds*1000)*2.5;
  let path='',previous=null;
  for(const row of rows){const v=row.values[key]; if(typeof v!=='number'||!Number.isFinite(v)){previous=null;continue;} const move=!previous||row.at-previous.at>maxGap;path+=`${move?'M':'L'}${x(row.at).toFixed(1)},${y(v).toFixed(1)} `;previous=row;}
  svg+=`<path d="${path}" fill="none" stroke="${color}" stroke-width="2" stroke-linejoin="round"/>`;
  if(good.length===1) svg+=`<circle cx="${x(good[0].at)}" cy="${y(good[0].values[key])}" r="3" fill="${color}"/>`;
  if(!good.length) svg+=`<text x="${width/2}" y="90" text-anchor="middle" font-size="12" fill="#91a092">No measurements for this date</text>`;
  return svg+'</svg>';
}
function renderCharts() {
  const inc=config.incubators.find(i=>i.id===$('chart-unit').value)||config.incubators[0];
  const rows=chartRows.filter(r=>r.incubatorId===inc.id).sort((a,b)=>a.at-b.at);
  $('charts').innerHTML=drawChart(rows,'temperature',inc.thresholds.temperatureLow,inc.thresholds.temperatureHigh,'#26745e')+drawChart(rows,'co2',inc.thresholds.co2Low,inc.thresholds.co2High,'#6784ba');
  $('chart-foot').textContent=`${rows.length} readings · ${mode==='demo'?'Simulated history':mode==='bridge'?'Service archive':'Browser archive'} · Local time shown; CSV timestamps use UTC`;
}
async function loadHistory() {
  if(chartBusy)return false; chartBusy=true;
  const selectedMode=mode, generation=session;
  try {
    const start=new Date($('chart-date').value+'T00:00:00'), end=new Date(start); end.setDate(end.getDate()+1);
    if(!Number.isFinite(start.getTime()))throw new Error('Select an archive date.');
    let rows;
    if(mode==='bridge') rows=await api(`history?since=${start.getTime()}&until=${end.getTime()-1}`);
    else if(mode==='demo') rows=demoRows.filter(r=>r.at>=start.getTime()&&r.at<end.getTime());
    else rows=archiveReady?await archive.rows(start.getTime(),end.getTime()-1):[];
    if(mode!==selectedMode||session!==generation)return false;
    chartRows=rows;renderCharts();
    return true;
  } catch(error){$('chart-foot').textContent='Archive unavailable: '+error.message;return false;}
  finally{chartBusy=false;}
}
async function pollDirect(inc, generation) {
  if(!running||generation!==session)return;
  const sample=await clients.get(inc.id).poll();
  if(!running||generation!==session)return;
  const now=Date.now();sample.at=now;sample.incubatorId=inc.id;sample.alarms=engines.get(inc.id).evaluate(sample,now);latest[inc.id]=sample;
  if(archiveReady&&now-(lastSave.get(inc.id)||0)>=config.saveMinutes*60000){
    try{await archive.add(sample);lastSave.set(inc.id,now);}catch(error){storageError='Archive write failed: '+error.message;}
  }
  render();await loadHistory();
  if(running&&generation===session)timers.set(inc.id,setTimeout(()=>pollDirect(inc,generation).catch(error=>toast(error.message)),config.pollSeconds*1000));
}
function pollDemo(generation) {
  if(!running||generation!==session)return;
  const now=Date.now(),phase=(now-demoStarted)/20000;
  for(const inc of config.incubators){const sample={at:now,incubatorId:inc.id,values:{temperature:37+Math.sin(phase+Number(inc.id))*.07,co2:4.8+Math.cos(phase+Number(inc.id))*.06,water:inc.id==='3'?0:1},errors:{}};sample.alarms=engines.get(inc.id).evaluate(sample,now);latest[inc.id]=sample;demoRows.push(sample);}
  demoRows=demoRows.filter(r=>r.at>now-86400000);render();loadHistory();timers.set('demo',setTimeout(()=>pollDemo(generation),2000));
}
async function startStop() {
  busy=true;render();
  try {
    if(mode==='bridge') {await api(running?'stop':'start',{});await refreshBridge();}
    else if(running) {
      running=false;session++;for(const timer of timers.values())clearTimeout(timer);timers.clear();
      for(const client of clients.values())await client.close();clients.clear();
    } else {
      if(mode==='direct'&&config.incubators.some(i=>!clients.has(i.id)))throw new Error('Connect a serial port for every configured incubator first.');
      if(mode==='direct'&&!archiveReady)throw new Error('Browser archive is unavailable. Use the background service or allow browser storage.');
      running=true;session++;lastSave.clear();
      engines=new Map(config.incubators.map(i=>[i.id,new AlarmEngine(config,i,eventReceived)]));
      if(mode==='demo')pollDemo(session);else for(const inc of config.incubators)pollDirect(inc,session).catch(error=>toast(error.message));
    }
  } catch(error){toast(error.message);}finally{busy=false;render();}
}
async function switchMode(next) {
  if(running||clients.size){toast('Stop monitoring and disconnect serial ports before changing mode.');$('mode').value=mode;return;}
  if(next==='bridge'&&!service){toast('Start backend/server.py and open http://127.0.0.1:8765 to use the background service.');$('mode').value=mode;return;}
  mode=next;session++;bridgeError='';latest={};chartRows=[];
  if(mode==='bridge'){config=clone(service.config);await refreshBridge();}
  else if(mode==='demo'){config=demoConfig();events=[];makeDemo();}
  else {config=clone(savedConfig);events=clone(realEvents);}
  updateChartUnits();render();await loadHistory();
}
async function refreshBridge() {
  if(mode!=='bridge'||bridgeBusy)return;
  bridgeBusy=true;
  try {
    const current=await api('status');
    if(mode!=='bridge')return;
    const oldNewest=events[0]?.at||0;
    service=current;config=clone(current.config);running=current.running;latest=current.latest;events=current.events;bridgeError='';
    if(notifications&&events.some(e=>e.type==='alarm'&&e.at>oldNewest)){beep();if('Notification'in window&&Notification.permission==='granted')new Notification('Incubator alarm',{body:events.find(e=>e.type==='alarm'&&e.at>oldNewest).message});}
    render();
  }catch(error){bridgeError='Background service connection lost';render();}finally{bridgeBusy=false;}
}

function fillSettings() {
  for(const key of ['pollSeconds','saveMinutes','retentionDays'])$('settings-form').elements[key].value=draft[key];
  $('delay-inputs').innerHTML=Object.entries(ALARM_LABELS).map(([key,label])=>`<label>${label}<input data-delay="${key}" type="number" min="0" max="1440" step="0.1" value="${draft.delays[key]}" required></label>`).join('');
  $('unit-inputs').innerHTML=draft.incubators.map((inc,index)=>`<div class="unit-form" data-unit="${index}"><div class="unit-form-head"><label>Name<input data-field="name" value="${escapeHtml(inc.name)}" maxlength="100" required></label>${mode==='bridge'?`<label>Serial port<input data-field="port" value="${escapeHtml(inc.port)}" list="ports" placeholder="COM3 or /dev/ttyUSB0"></label>`:''}<label>Baud rate<input data-field="baud" type="number" min="300" max="1000000" step="1" value="${inc.baud}" required></label><button type="button" data-remove="${index}" ${draft.incubators.length===1?'disabled':''}>Remove</button></div><div class="form-grid four">${[['temperatureLow','Temp min, °C'],['temperatureHigh','Temp max, °C'],['co2Low','CO₂ min, %'],['co2High','CO₂ max, %'],['waterLow','Water low threshold']].map(([key,label])=>`<label>${label}<input data-threshold="${key}" type="number" min="0" max="1000" step="0.01" value="${inc.thresholds[key]}" required></label>`).join('')}</div></div>`).join('');
  $('add-unit').disabled=draft.incubators.length>=9;
}
function collectSettings() {
  for(const key of ['pollSeconds','saveMinutes','retentionDays'])draft[key]=Number($('settings-form').elements[key].value);
  document.querySelectorAll('[data-delay]').forEach(input=>draft.delays[input.dataset.delay]=Number(input.value));
  document.querySelectorAll('[data-unit]').forEach(form=>{
    const inc=draft.incubators[Number(form.dataset.unit)];
    form.querySelectorAll('[data-field]').forEach(input=>inc[input.dataset.field]=input.dataset.field==='baud'?Number(input.value):input.value);
    form.querySelectorAll('[data-threshold]').forEach(input=>inc.thresholds[input.dataset.threshold]=Number(input.value));
  });
}
async function openSettings() {
  if(running||clients.size)return;
  if(service?.demo&&mode==='bridge'){toast('Demo service uses temporary settings. Restart without --demo to configure hardware.');return;}
  draft=clone(config);fillSettings();$('settings').showModal();$('ports-note').textContent=mode==='direct'?'Chrome will ask you to select each port using Connect serial.':'Use a stable port identifier if USB adapters are reconnected.';
  if(mode==='bridge'){try{const result=await api('ports');$('ports').innerHTML=result.ports.map(p=>`<option value="${escapeHtml(p.port)}">${escapeHtml(p.name)}</option>`).join('');if(result.error)$('ports-note').textContent=result.error;}catch(error){$('ports-note').textContent=error.message;}}
}

$('run').addEventListener('click',startStop);
$('mode').addEventListener('change',e=>switchMode(e.target.value));
$('cards').addEventListener('click',async e=>{
  const button=e.target.closest('[data-connect]');if(!button||running)return;
  const id=button.dataset.connect;button.disabled=true;
  try{
    if(clients.has(id)){await clients.get(id).close();clients.delete(id);}
    else{
      if(!('serial'in navigator))throw new Error('Use desktop Chrome at localhost or HTTPS for serial access.');
      const port=await navigator.serial.requestPort();
      if([...clients.values()].some(c=>c.port===port))throw new Error('This port is already assigned to another incubator.');
      const client=new SerialClient(port);await client.open(config.incubators.find(i=>i.id===id).baud);clients.set(id,client);
    }
  }catch(error){if(error.name!=='NotFoundError')toast(error.message);}finally{render();}
});
$('notifications').addEventListener('click',async()=>{
  try{audio ||= new AudioContext();await audio.resume();notifications=true;beep();if('Notification'in window&&Notification.permission==='default')await Notification.requestPermission();$('notifications').textContent='Sound enabled';}catch(error){toast(error.message);}
});
$('chart-unit').addEventListener('change',renderCharts);$('chart-date').addEventListener('change',loadHistory);
$('export').addEventListener('click',async()=>{if(!await loadHistory()){toast('Archive not ready. Wait for loading to finish or check the service.');return;}download(`incubator-${mode==='demo'||service?.demo&&mode==='bridge'?'DEMO-':''}${$('chart-date').value}.csv`,csvText(chartRows,config.incubators),'text/csv;charset=utf-8');});
$('settings-open').addEventListener('click',openSettings);
for(const id of ['settings-close','settings-cancel'])$(id).addEventListener('click',()=>$('settings').close());
$('add-unit').addEventListener('click',()=>{collectSettings();let id=1;while(draft.incubators.some(i=>i.id===String(id)))id++;draft.incubators.push({...clone(DEFAULTS.incubators[0]),id:String(id),name:'Incubator '+id});fillSettings();});
$('unit-inputs').addEventListener('click',e=>{const button=e.target.closest('[data-remove]');if(button&&draft.incubators.length>1){collectSettings();draft.incubators.splice(Number(button.dataset.remove),1);fillSettings();}});
$('settings-form').addEventListener('submit',async e=>{
  e.preventDefault();collectSettings();
  try{
    validateConfig(draft);
    if(mode==='bridge'){await api('config',draft);await refreshBridge();}
    else if(mode==='direct'){localStorage.setItem('incubator-config-v1',JSON.stringify(draft));savedConfig=clone(draft);config=clone(draft);if(archiveReady)await archive.prune(Date.now()-config.retentionDays*86400000);}
    else{config=clone(draft);events=[];latest={};makeDemo();}
    $('settings').close();updateChartUnits();render();await loadHistory();toast('Settings saved.');
  }catch(error){toast(error.message);}
});
document.addEventListener('visibilitychange',render);
window.addEventListener('beforeunload',e=>{if(mode==='direct'&&(running||clients.size)){e.preventDefault();e.returnValue='';}});

async function init() {
  $('chart-date').value=today();
  try{await archive.open();archiveReady=true;await archive.prune(Date.now()-savedConfig.retentionDays*86400000);}catch(error){storageError='Browser storage unavailable: '+error.message;}
  if(location.protocol==='http:'||location.protocol==='https:'){
    try{service=await api('status');token=(await api('session')).token;mode='bridge';config=clone(service.config);latest=service.latest;events=service.events;running=service.running;}catch{}
  }
  if(mode==='demo'){config=demoConfig();events=[];makeDemo();}
  updateChartUnits();render();await loadHistory();
  if(mode==='demo')await startStop();
  setInterval(()=>{if(mode==='bridge')refreshBridge();else render();},2000);
  setInterval(loadHistory,15000);
  setInterval(()=>{if(mode==='direct'&&archiveReady)archive.prune(Date.now()-config.retentionDays*86400000).catch(error=>{storageError='Archive cleanup failed: '+error.message;render();});},3600000);
}
init().catch(error=>toast(error.message));

