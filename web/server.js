const express = require('express');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

const ROOT = path.join(__dirname, '..');

function runOrbit(input) {
  return new Promise((resolve, reject) => {
    const p = spawn(path.join(ROOT, 'orbit.exe'), [], { cwd: ROOT });
    let out = '';
    p.stdout.on('data', d => (out += d));
    p.on('error', reject);
    p.on('close', () => resolve(out));
    p.stdin.write(input);
    p.stdin.end();
  });
}

app.get('/api/ping', (req, res) => res.json({ ok: true }));

app.get('/api/items', async (req, res) => {
  try {
    res.json({ output: await runOrbit('3\n0\n') });
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});
app.post('/api/report', async (req, res) => {
  try {
    const clean = s => String(s || '').replace(/[\r\n]+/g, ' ');
    const { type, title, category, description, location, datetime } = req.body;
    const choice = type === 'found' ? '2' : '1';
    const input = [choice, title, category, description, location, datetime, '0']
      .map(clean).join('\n') + '\n';
    res.json({ output: await runOrbit(input) });
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});
const fs = require('fs');

app.get('/api/data', (req, res) => {
  try {
    const text = fs.readFileSync(path.join(ROOT, 'orbit_data.txt'), 'utf8');
    const re = /^(\d+)\s+"([^"]*)"\s+"([^"]*)"\s+"([^"]*)"\s+"([^"]*)"\s+"([^"]*)"\s+"([^"]*)"\s+(\d)/;
    const items = text.split(/\r?\n/).map(l => l.match(re)).filter(Boolean).map(m => ({
      id: Number(m[1]),
      type: m[2],
      title: m[3],
      category: m[4],
      description: m[5],
      location: m[6],
      datetime: m[7],
      resolved: m[8] === '1'
    }));
    res.json(items);
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});
app.post('/api/match', async (req, res) => {
  try {
    const id = String(Number(req.body.id) || 0);
    const out = await runOrbit(['4', id, '0'].join('\n') + '\n');
    res.json({ output: out });
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});
app.post('/api/match', async (req, res) => {
  try {
    const id = String(Number(req.body.id) || 0);
    const out = await runOrbit(['4', id, '0'].join('\n') + '\n');
    res.json({ output: out });
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});
app.post('/api/resolve', async (req, res) => {
  try {
    const id = String(Number(req.body.id) || 0);
    const out = await runOrbit(['5', id, '0'].join('\n') + '\n');
    res.json({ output: out });
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});
app.listen(3000, () => console.log('Running at http://localhost:3000'));