'use strict';
const $ = id => document.getElementById(id);
const stages = [
 ['tx-app', 'ECU 1 · Application', 'The application provides VehicleSpeed to COM.', 'Com_SendSignal(0, &value)'],
 ['tx-com', 'ECU 1 · COM', 'COM packs the 16-bit signal into two bytes, low byte first.', 'COM packs the signal → PDU 10'],
 ['tx-pdur', 'ECU 1 · PduR', 'The routing table maps COM transmit PDU 10 to CanIf PDU 20.', 'PduR_ComTransmit(10, &pdu)'],
 ['tx-canif', 'ECU 1 · CanIf', 'CanIf resolves CAN ID 0x123 and passes the frame to the simulated driver.', 'CanIf_Transmit(20, &pdu)'],
 ['bus', 'CAN · Frame in transit', 'The simulated bus buffers the frame. A successful driver return confirms ECU 1 transmission.', 'CAN 0x123 → simulated bus → ECU 2'],
 ['rx-canif', 'ECU 2 · CanIf', 'A fresh receiver lifecycle matches CAN ID 0x123 and the two-byte length to receive PDU 21.', 'CanIf_RxIndication(0x123, &pdu)'],
 ['rx-pdur', 'ECU 2 · PduR', 'PduR routes CanIf receive PDU 21 to COM receive PDU 11.', 'PduR_CanIfRxIndication(21, &pdu)'],
 ['rx-com', 'ECU 2 · COM', 'COM decodes the little-endian bytes and stores the received signal.', 'Com_RxIndication(11, &pdu)'],
 ['rx-app', 'ECU 2 · Application', 'The receiver reads VehicleSpeed. The received value matches the transmitted value.', 'Com_ReceiveSignal(0, &value)']
];
let result = null, index = -1, timer = null, playing = false, loading = false, generation = 0;
const hex = (n, width = 2) => n.toString(16).toUpperCase().padStart(width, '0');
function buttons() {
 $('send').disabled = loading || (result !== null && index < stages.length - 1);
 $('value').disabled = $('send').disabled;
 $('pause').disabled = !result || index >= stages.length - 1;
 $('pause').textContent = playing ? 'Pause' : 'Resume';
 $('step').disabled = loading || (result !== null && index >= stages.length - 1);
}
function clear() {
 generation++; clearTimeout(timer); playing = false; loading = false; result = null; index = -1;
 document.querySelectorAll('.active,.visited').forEach(el => el.classList.remove('active', 'visited'));
 for (const id of ['byte0', 'byte1', 'sent', 'received', 'confirmed']) $(id).textContent = '—';
 $('trace').replaceChildren(); $('progress').textContent = 'Ready to transmit';
 $('stage-title').textContent = 'Your signal’s journey starts here.';
 $('stage-description').textContent = 'Set a value and press Send signal, or use Step to explore the path.';
 $('step-count').textContent = '0 / 9'; buttons();
}
function advance() {
 if (!result || index >= stages.length - 1) return;
 if (index >= 0) { $(stages[index][0]).classList.remove('active'); $(stages[index][0]).classList.add('visited'); }
 index++;
 const [id, title, description, call] = stages[index];
 $(id).classList.add('active'); $('stage-title').textContent = title;
 $('stage-description').textContent = description;
 $('step-count').textContent = `${index + 1} / ${stages.length}`;
 $('progress').textContent = index === 8 ? '✓ Signal delivered' : `Step ${index + 1} of 9`;
 const event = document.createElement('li'); event.textContent = call;
 $('trace').append(event); $('trace').scrollTop = $('trace').scrollHeight;
 if (index === 0) $('sent').textContent = result.sent;
 if (index === 1) {
  $('byte0').textContent = hex(result.bytes[0]); $('byte1').textContent = hex(result.bytes[1]);
  $('packet').textContent = result.bytes.map(b => hex(b)).join(' ') + ' →';
  $('can-id').textContent = '0x' + hex(result.canId, 3);
 }
 if (index === 4) $('confirmed').textContent = result.confirmations;
 if (index === 8) { $('received').textContent = result.received; playing = false; }
 buttons();
}
function schedule() {
 clearTimeout(timer);
 if (playing) timer = setTimeout(() => { advance(); schedule(); }, Number($('speed').value));
}
async function start(autoplay) {
 if (!$('controls').reportValidity()) return;
 clear(); const requestGeneration = generation;
 loading = true; $('progress').textContent = 'Running C demo…'; buttons();
 try {
  const response = await fetch('/api/transmit', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({value: Number($('value').value)}) });
  const data = await response.json();
  if (requestGeneration !== generation) return;
  if (!response.ok) throw new Error(data.error || 'Transmission failed');
  result = data; loading = false; playing = autoplay; advance(); schedule();
 } catch (error) {
  if (requestGeneration !== generation) return;
  loading = false; $('progress').textContent = 'Transmission failed';
  $('stage-title').textContent = 'Could not run the C demo';
  $('stage-description').textContent = error.message; buttons();
 }
}
$('controls').addEventListener('submit', event => { event.preventDefault(); start(true); });
$('pause').addEventListener('click', () => { playing = !playing; buttons(); schedule(); });
$('step').addEventListener('click', () => { if (!result) { start(false); return; } playing = false; clearTimeout(timer); advance(); });
$('reset').addEventListener('click', clear);
$('speed').addEventListener('change', schedule);
clear();
