#include "webDashboard.h"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <string.h>

namespace {
WebServer server(80);
portMUX_TYPE snapshotMux = portMUX_INITIALIZER_UNLOCKED;
WebDashboardSnapshot latestSnapshot = {};

const char dashboardPage[] PROGMEM = R"dashboardhtml(
<!doctype html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="theme-color" content="#080b14">
  <title>Cripto Miner</title>
  <style>
    :root{color-scheme:dark;--bg:#080b14;--panel:#101625;--panel2:#151d2d;--text:#edf7ff;--muted:#91a2ba;--cyan:#43f5ff;--pink:#ff3fd2;--violet:#9665ff;--green:#69ffc1;--line:#28334b}
    *{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at 8% 0,#18234a 0,transparent 38%),radial-gradient(ellipse at 100% 18%,#35133d66 0,transparent 34%),linear-gradient(180deg,#080b14,#0a0d18 72%,#090b13);color:var(--text);font:15px/1.45 system-ui,-apple-system,Segoe UI,sans-serif;min-height:100vh}body:before{content:"";position:fixed;inset:0;pointer-events:none;opacity:.14;background-image:linear-gradient(#43f5ff0a 1px,transparent 1px),linear-gradient(90deg,#43f5ff0a 1px,transparent 1px);background-size:38px 38px;mask-image:linear-gradient(to bottom,black,transparent 88%)}
    .wrap{position:relative;max-width:1100px;margin:auto;padding:28px 20px 42px}.top{display:flex;align-items:center;justify-content:space-between;gap:16px;margin-bottom:24px}.brand{font-weight:900;font-size:23px;letter-spacing:.08em;text-shadow:0 0 20px #43f5ff55}.brand span{color:var(--cyan);text-shadow:0 0 15px #43f5ff}.sub{display:block;color:var(--pink);font-size:10px;font-weight:700;letter-spacing:.22em;margin-top:3px;text-transform:uppercase}.state{border:1px solid var(--line);background:#101625cc;border-radius:99px;padding:8px 13px;color:var(--muted);font-size:12px;white-space:nowrap;box-shadow:inset 0 0 16px #43f5ff08}.dot{display:inline-block;width:8px;height:8px;border-radius:50%;background:var(--muted);margin-right:7px}.state.on{border-color:#69ffc155;color:var(--green)}.state.on .dot{background:var(--green);box-shadow:0 0 12px #69ffc1}
    nav{display:flex;gap:9px;overflow:auto;padding:2px 1px 14px;margin-bottom:7px}button{border:1px solid var(--line);background:#101625d9;color:var(--muted);border-radius:8px;padding:10px 16px;font:700 13px system-ui;cursor:pointer;white-space:nowrap;transition:.2s}button.active,button:hover{border-color:var(--cyan);color:var(--cyan);background:#43f5ff12;box-shadow:0 0 18px #43f5ff18}.panel{display:none}.panel.active{display:block;animation:appear .25s ease-out}@keyframes appear{from{opacity:.5;transform:translateY(5px)}to{opacity:1;transform:none}}.hero{background:linear-gradient(112deg,#10223a,#1c1740 58%,#351332);border:1px solid #43f5ff77;color:var(--text);border-radius:14px;padding:23px 25px;margin-bottom:14px;display:flex;align-items:center;justify-content:space-between;gap:16px;box-shadow:0 0 28px #43f5ff13,inset 0 0 30px #9665ff0c}.hero label{display:block;color:var(--cyan);font-size:11px;font-weight:800;text-transform:uppercase;letter-spacing:.16em}.hero strong{display:block;font-size:clamp(28px,6vw,44px);line-height:1.15;margin-top:5px;text-shadow:0 0 18px #43f5ff35}.hero .right{text-align:right;font-weight:700}.hero .right small{color:var(--muted)}.grid{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:12px}.card{position:relative;overflow:hidden;background:linear-gradient(145deg,#151d2dee,#0e1421f2);border:1px solid #28334b;border-radius:11px;padding:16px;min-height:98px;box-shadow:inset 0 1px #ffffff08;transition:border-color .2s,transform .2s}.card:after{content:"";position:absolute;top:0;left:15px;width:34px;height:1px;background:var(--cyan);box-shadow:0 0 9px var(--cyan)}.card:hover{border-color:#43f5ff66;transform:translateY(-2px)}.card label{display:block;color:var(--muted);font-size:11px;margin-bottom:8px;text-transform:uppercase;letter-spacing:.08em}.card strong{font-size:20px;letter-spacing:-.02em;overflow-wrap:anywhere;color:#f3f8ff}.card small{color:var(--muted);font-size:11px}.wide{grid-column:span 2}.progress{height:8px;border-radius:9px;background:#080b14;margin:12px 0 8px;overflow:hidden;border:1px solid #28334b}.progress i{height:100%;display:block;background:linear-gradient(90deg,var(--violet),var(--pink),var(--cyan));width:0%;border-radius:9px;box-shadow:0 0 14px #43f5ff88;transition:width .6s}.footer{display:flex;justify-content:space-between;color:var(--muted);font-size:11px;margin-top:18px;gap:12px;flex-wrap:wrap}.footer a{color:var(--cyan);text-decoration:none}.footer a:hover{text-decoration:underline}.error{display:none;background:#5c183b;border:1px solid var(--pink);color:#ffe7fa;padding:12px 14px;border-radius:9px;margin-bottom:14px}.error.show{display:block}.note{color:var(--muted);font-size:11px;margin:13px 2px 0}
    @media(max-width:760px){.grid{grid-template-columns:repeat(2,minmax(0,1fr))}.wrap{padding:20px 13px 30px}.card{padding:14px}.hero{padding:18px}.card strong{font-size:18px}}
    @media(max-width:390px){.brand{font-size:18px}.state{font-size:11px;padding:7px 9px}.card{min-height:88px}.card strong{font-size:16px}.wide{grid-column:span 2}}
  </style>
</head>
<body>
  <main class="wrap">
    <header class="top"><div class="brand"><span>₿</span> CRIPTO MINER<small class="sub">dev by Brskyy</small></div><div class="state" id="state"><i class="dot"></i><span id="status">aguardando dados</span></div></header>
    <div class="error" id="error">Não foi possível ler os dados do minerador. Verifique se o ESP32 continua ligado à mesma rede Wi-Fi.</div>
    <nav aria-label="Telas do minerador"><button class="active" data-page="mine">Mineração</button><button data-page="clock">Relógio</button><button data-page="global">Estatísticas globais</button></nav>
    <section class="panel active" id="mine">
      <div class="hero"><div><label>Taxa de hash atual</label><strong><span data-key="hashRate">—</span> <small>KH/s</small></strong></div><div class="right"><div id="mineStatus">—</div><small>desde <span data-key="timeMining">—</span></small></div></div>
      <div class="grid">
        <div class="card"><label>Shares processadas</label><strong data-key="completedShares">—</strong></div><div class="card"><label>Blocos válidos</label><strong data-key="valids">—</strong></div>
        <div class="card"><label>Melhor dificuldade</label><strong data-key="bestDifficulty">—</strong></div><div class="card"><label>Templates recebidos</label><strong data-key="templates">—</strong></div>
        <div class="card"><label>Total de hashes</label><strong><span data-key="totalMHashes">—</span> <small>MH</small></strong></div><div class="card"><label>Temperatura estimada</label><strong><span data-key="temperature">—</span> <small>°C</small></strong></div>
        <div class="card"><label>ESP32 configurado</label><strong data-key="workersCount">—</strong><small>nesta carteira</small></div><div class="card"><label>Hashrate deste ESP32</label><strong><span data-key="workersHash">—</span></strong></div>
        <div class="card wide"><label>Melhor dificuldade local</label><strong data-key="poolBestDifficulty">—</strong></div><div class="card wide"><label>Conexão</label><strong data-key="ipAddress">—</strong><small id="wifiState"></small></div>
      </div>
    </section>
    <section class="panel" id="clock">
      <div class="hero"><div><label>Hora local</label><strong data-key="currentTime">--:--</strong></div><div class="right"><div data-key="currentDate">—</div><small>fuso configurado no minerador</small></div></div>
      <div class="grid"><div class="card"><label>Bitcoin · cotação ao vivo</label><strong id="btcPriceBRL">Carregando…</strong><small id="btcPriceUSD">USD: —</small></div><div class="card"><label>Altura atual do bloco</label><strong id="blockHeight">—</strong></div><div class="card"><label>Tempo de mineração</label><strong data-key="timeMining">—</strong></div><div class="card"><label>Hashrate deste ESP32</label><strong><span data-key="hashRate">—</span> <small>KH/s</small></strong></div></div>
    </section>
    <section class="panel" id="global">
      <div class="grid"><div class="card"><label>Hashrate da rede</label><strong id="networkHashRate">Carregando…</strong></div><div class="card"><label>Dificuldade da rede</label><strong id="networkDifficulty">—</strong></div><div class="card"><label>Taxa estimada · 30 min</label><strong id="halfHourFee">—</strong><small>sat/vB</small></div><div class="card"><label>Bloco atual</label><strong id="globalBlockHeight">—</strong></div>
        <div class="card"><label>Transações aguardando</label><strong id="mempoolCount">—</strong></div><div class="card"><label>Tamanho da mempool</label><strong id="mempoolSize">—</strong></div>
        <div class="card wide"><label>Progresso do ajuste de dificuldade</label><strong><span id="difficultyProgress">—</span></strong><div class="progress"><i id="progress"></i></div><small id="difficultyEstimate">Aguardando dados da rede</small></div>
        <div class="card wide"><label>Progresso até o próximo halving</label><strong><span id="progressPercent">—</span>%</strong><div class="progress"><i id="halvingProgress"></i></div><small><span id="remainingBlocks">—</span> blocos restantes · estimativa pela altura</small></div>
        <div class="card wide"><label>Estatísticas locais</label><strong><span data-key="totalKHashes">—</span> <small>KH acumulados</small></strong><small style="display:block;margin-top:7px">Atualizadas pelo próprio ESP32</small></div>
      </div>
      <p class="note">Dados de mercado e rede atualizados automaticamente a cada 60 segundos; preço em reais é uma referência informativa.</p>
    </section>
    <footer class="footer"><span>Mineração atualizada a cada 5 segundos · rede a cada 60 segundos · <span id="globalUpdated">aguardando atualização</span></span><span>IP do ESP32: <b data-key="ipAddress">—</b> · dados: <a href="https://mempool.space" target="_blank" rel="noopener">mempool.space</a></span></footer>
  </main>
  <script>
    const keys=['hashRate','timeMining','completedShares','valids','bestDifficulty','templates','totalMHashes','temperature','workersCount','workersHash','poolBestDifficulty','ipAddress','currentTime','currentDate','totalKHashes'];
    document.querySelectorAll('nav button').forEach(b=>b.addEventListener('click',()=>{document.querySelectorAll('nav button').forEach(x=>x.classList.toggle('active',x===b));document.querySelectorAll('.panel').forEach(x=>x.classList.toggle('active',x.id===b.dataset.page));}));
    async function refresh(){try{const r=await fetch('/api/state',{cache:'no-store'});if(!r.ok)throw new Error('HTTP '+r.status);const d=await r.json();document.getElementById('error').classList.remove('show');keys.forEach(k=>document.querySelectorAll('[data-key="'+k+'"]').forEach(e=>e.textContent=d[k]??'—'));document.getElementById('status').textContent=d.status||'conectado';document.getElementById('state').classList.toggle('on',!!d.wifiConnected);document.getElementById('mineStatus').textContent=d.status||'—';document.getElementById('wifiState').textContent=d.wifiConnected?'Wi-Fi conectado':'Wi-Fi desconectado';}catch(e){document.getElementById('error').classList.add('show');document.getElementById('state').classList.remove('on');document.getElementById('status').textContent='sem conexão';}}
    async function getJson(path){const controller=new AbortController();const timeout=setTimeout(()=>controller.abort(),10000);try{const response=await fetch('https://mempool.space'+path,{cache:'no-store',signal:controller.signal});if(!response.ok)throw new Error('HTTP '+response.status);return await response.json();}finally{clearTimeout(timeout)}}
    async function getText(path){const controller=new AbortController();const timeout=setTimeout(()=>controller.abort(),10000);try{const response=await fetch('https://mempool.space'+path,{cache:'no-store',signal:controller.signal});if(!response.ok)throw new Error('HTTP '+response.status);return (await response.text()).trim()}finally{clearTimeout(timeout)}}
    const set=(id,value)=>{if(value!==undefined&&value!==null&&value!==''&&!Number.isNaN(value))document.getElementById(id).textContent=value};
    const numberFormat=new Intl.NumberFormat('pt-BR',{maximumFractionDigits:0});
    function formatHashrate(value){if(!Number.isFinite(value)||value<=0)return null;const units=['H/s','KH/s','MH/s','GH/s','TH/s','PH/s','EH/s','ZH/s'];let i=0;while(value>=1000&&i<units.length-1){value/=1000;i++}return value.toLocaleString('pt-BR',{maximumFractionDigits:2})+' '+units[i]}
    async function refreshGlobal(){const paths=['/api/v1/prices','/api/blocks/tip/height','/api/v1/mining/hashrate/3d','/api/v1/difficulty-adjustment','/api/v1/fees/recommended','/api/mempool'];const results=await Promise.allSettled([getJson(paths[0]),getText(paths[1]),getJson(paths[2]),getJson(paths[3]),getJson(paths[4]),getJson(paths[5])]);let updated=false;
      const price=results[0];if(price.status==='fulfilled'){const p=price.value;if(Number.isFinite(Number(p.BRL))){set('btcPriceBRL',new Intl.NumberFormat('pt-BR',{style:'currency',currency:'BRL',maximumFractionDigits:0}).format(p.BRL));updated=true}if(Number.isFinite(Number(p.USD)))set('btcPriceUSD','USD: '+new Intl.NumberFormat('en-US',{style:'currency',currency:'USD',maximumFractionDigits:0}).format(p.USD))}
      const height=results[1];if(height.status==='fulfilled'){const h=Number(height.value);if(Number.isFinite(h)){set('blockHeight',numberFormat.format(h));set('globalBlockHeight',numberFormat.format(h));updated=true}}
      const mining=results[2];if(mining.status==='fulfilled'){const m=mining.value;const history=m.hashrates||m;const last=Array.isArray(history)?history[history.length-1]:null;const rate=Number(m.currentHashrate??(last&&last.avgHashrate));const diff=Number(m.currentDifficulty??(last&&last.difficulty));const rateText=formatHashrate(rate);if(rateText){set('networkHashRate',rateText);updated=true}if(Number.isFinite(diff)&&diff>0){set('networkDifficulty',numberFormat.format(diff));updated=true}}
      const adjustment=results[3];if(adjustment.status==='fulfilled'){const a=adjustment.value;if(Number.isFinite(Number(a.progressPercent))){set('difficultyProgress',Number(a.progressPercent).toLocaleString('pt-BR',{maximumFractionDigits:1})+'%');document.getElementById('progress').style.width=Math.max(0,Math.min(100,Number(a.progressPercent)))+'%';updated=true}if(Number.isFinite(Number(a.difficultyChange))){set('difficultyEstimate','Próximo ajuste estimado: '+Number(a.difficultyChange).toLocaleString('pt-BR',{maximumFractionDigits:2})+'% · '+numberFormat.format(Number(a.remainingBlocks)||0)+' blocos restantes');set('remainingBlocks',numberFormat.format(Number(a.remainingBlocks)||0));updated=true}}
      const fees=results[4];if(fees.status==='fulfilled'&&Number.isFinite(Number(fees.value.halfHourFee))){set('halfHourFee',numberFormat.format(Number(fees.value.halfHourFee)));updated=true}
      const pool=results[5];if(pool.status==='fulfilled'){const m=pool.value;if(Number.isFinite(Number(m.count)))set('mempoolCount',numberFormat.format(Number(m.count)));if(Number.isFinite(Number(m.vsize)))set('mempoolSize',(Number(m.vsize)/1e6).toLocaleString('pt-BR',{maximumFractionDigits:1})+' vMB');updated=true}
      if(Number.isFinite(Number(document.getElementById('blockHeight').textContent.replaceAll('.','')))){const current=Number(document.getElementById('blockHeight').textContent.replaceAll('.',''));const interval=210000;const remaining=interval-(current%interval);set('progressPercent',((current%interval)*100/interval).toLocaleString('pt-BR',{maximumFractionDigits:2}));set('remainingBlocks',numberFormat.format(remaining));document.getElementById('halvingProgress').style.width=((current%interval)*100/interval)+'%'}
      if(updated)set('globalUpdated','última atualização '+new Date().toLocaleTimeString('pt-BR',{hour:'2-digit',minute:'2-digit',second:'2-digit'}));else set('globalUpdated','API indisponível · tentando novamente em 60 s')}
    refresh();refreshGlobal();setInterval(refresh,5000);setInterval(refreshGlobal,60000);
  </script>
</body>
</html>
)dashboardhtml";
}

void setWebDashboardSnapshot(const WebDashboardSnapshot &snapshot) {
  portENTER_CRITICAL(&snapshotMux);
  memcpy(&latestSnapshot, &snapshot, sizeof(latestSnapshot));
  portEXIT_CRITICAL(&snapshotMux);
}

void startWebDashboard() {
  if (WiFi.status() != WL_CONNECTED) return;

  server.on("/", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-store");
    server.send_P(200, "text/html; charset=utf-8", dashboardPage);
  });
  server.on("/api/state", HTTP_GET, []() {
    WebDashboardSnapshot snapshot;
    portENTER_CRITICAL(&snapshotMux);
    memcpy(&snapshot, &latestSnapshot, sizeof(snapshot));
    portEXIT_CRITICAL(&snapshotMux);

    StaticJsonDocument<1024> doc;
    doc["wifiConnected"] = snapshot.wifiConnected;
    doc["walletConfigured"] = snapshot.walletConfigured;
    doc["status"] = snapshot.status;
    doc["ipAddress"] = snapshot.ipAddress;
    doc["hashRate"] = snapshot.hashRate;
    doc["completedShares"] = snapshot.completedShares;
    doc["totalKHashes"] = snapshot.totalKHashes;
    doc["totalMHashes"] = snapshot.totalMHashes;
    doc["templates"] = snapshot.templates;
    doc["bestDifficulty"] = snapshot.bestDifficulty;
    doc["timeMining"] = snapshot.timeMining;
    doc["valids"] = snapshot.valids;
    doc["temperature"] = snapshot.temperature;
    doc["currentTime"] = snapshot.currentTime;
    doc["currentDate"] = snapshot.currentDate;
    doc["btcPrice"] = snapshot.btcPrice;
    doc["blockHeight"] = snapshot.blockHeight;
    doc["networkDifficulty"] = snapshot.networkDifficulty;
    doc["globalHashRate"] = snapshot.globalHashRate;
    doc["halfHourFee"] = snapshot.halfHourFee;
    doc["remainingBlocks"] = snapshot.remainingBlocks;
    doc["progressPercent"] = snapshot.progressPercent;
    doc["workersCount"] = snapshot.workersCount;
    doc["workersHash"] = snapshot.workersHash;
    doc["poolBestDifficulty"] = snapshot.poolBestDifficulty;

    String response;
    serializeJson(doc, response);
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json; charset=utf-8", response);
  });
  server.onNotFound([]() { server.send(404, "text/plain; charset=utf-8", "Nao encontrado"); });
  server.begin();
  Serial.printf("Web dashboard: http://%s/\n", WiFi.localIP().toString().c_str());
}

void handleWebDashboard() {
  server.handleClient();
}
