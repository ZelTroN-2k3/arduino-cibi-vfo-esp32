#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

const char* WEB_PAGE = R"=====(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" href="data:image/svg+xml,<svg xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 100%22><text y=%22.9em%22 font-size=%2290%22>📻</text></svg>">
    <title>VFO Digital Pro - Remote Control</title>
    <style>
        @import url('https://fonts.cdnfonts.com/css/seven-segment');
        
        :root {
            --bg-color: #0f1215;
            --panel-color: #1c2128;
            --accent-color: #00f2ff;
            --text-color: #e6edf3;
            --led-green: #2ecc71;
            --led-red: #e74c3c;
            --lcd-bg: #050a05;
            
            /* Theme dynamic colors */
            --theme-color: #2ecc71; /* Green by default */
            --theme-glow: rgba(46, 204, 113, 0.7);
        }

        body { 
            font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; 
            background-color: var(--bg-color); 
            color: var(--text-color); 
            margin: 0; 
            padding: 10px;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }

        .radio-unit {
            width: 100%;
            max-width: 450px;
            background: var(--panel-color);
            border-radius: 20px;
            padding: 25px;
            box-shadow: 0 20px 50px rgba(0,0,0,0.8), inset 0 1px 1px rgba(255,255,255,0.1);
            border: 1px solid #30363d;
            position: relative;
        }

        .conn-status {
            font-size: 0.6em;
            padding: 2px 8px;
            border-radius: 10px;
            background: #161b22;
            color: #8b949e;
            transition: all 0.3s;
            border: 1px solid #30363d;
        }
        .conn-status.online { color: var(--led-green); border: 1px solid var(--led-green); }
        .conn-status.offline { color: var(--led-red); border: 1px solid var(--led-red); }

        .header {
            margin-bottom: 20px;
        }

        .header-top {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 10px;
        }

        .header-bottom {
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .brand {
            font-weight: 800;
            letter-spacing: 2px;
            color: var(--accent-color);
            font-size: 0.9em;
            text-transform: uppercase;
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .settings-btn {
            cursor: pointer;
            color: #484f58;
            transition: color 0.2s;
        }
        .settings-btn:hover { 
            color: var(--accent-color); 
        }

        .status-indicators {
            display: flex;
            gap: 15px;
        }

        .config-panel {
            display: none;
            background: #161b22;
            border-radius: 10px;
            padding: 15px;
            margin-top: 15px;
            border: 1px solid #30363d;
            font-size: 0.8em;
        }

        .config-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            margin-top: 10px;
        }

        .config-item {
            display: flex;
            flex-direction: column;
            gap: 5px;
        }

        .config-item label { color: #8b949e; font-size: 0.9em; }
        .config-item input, .config-item select {
            background: #0d1117;
            border: 1px solid #30363d;
            color: var(--theme-color);
            padding: 5px;
            border-radius: 4px;
            font-family: monospace;
        }

        .toggle-group {
            display: flex;
            background: #0d1117;
            border: 1px solid #30363d;
            border-radius: 6px;
            overflow: hidden;
        }

        .toggle-btn {
            flex: 1;
            padding: 8px 0 !important;
            font-size: 0.8em !important;
            border: none !important;
            border-radius: 0 !important;
            background: transparent !important;
            box-shadow: none !important;
            color: #484f58 !important;
            margin: 0 !important;
        }

        .toggle-btn.active {
            background: var(--theme-color) !important;
            color: #050a05 !important;
            font-weight: 800;
        }

        .config-item input:focus, .config-item select:focus {
            outline: none;
            border-color: var(--accent-color);
        }

        .config-actions {
            margin-top: 15px;
            display: flex;
            justify-content: flex-end;
        }

        .status-indicators {
            display: flex;
            align-items: center;
            gap: 5px;
            font-size: 0.7em;
            font-weight: bold;
            color: #8b949e;
        }

        .led-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: #30363d;
            box-shadow: 0 0 2px rgba(0,0,0,0.5);
        }

        .led.active-rx .led-dot { 
            background: var(--led-green); 
            box-shadow: 0 0 10px var(--led-green); 
        }

        .led.active-tx .led-dot { 
            background: var(--led-red); 
            box-shadow: 0 0 10px var(--led-red); 
        }

        .display-screen {
            background: var(--lcd-bg);
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 25px;
            border: 4px solid #101418;
            box-shadow: inset 0 0 20px rgba(0,0,0,1);
            position: relative;
            overflow: hidden;
        }

        .display-screen::after {
            content: "";
            position: absolute;
            top: 0; left: 0; right: 0; bottom: 0;
            background: linear-gradient(rgba(18, 16, 16, 0) 50%, rgba(0, 0, 0, 0.1) 50%), 
                        linear-gradient(90deg, rgba(255, 0, 0, 0.03), rgba(0, 255, 0, 0.01), rgba(0, 0, 255, 0.03));
            background-size: 100% 3px, 3px 100%;
            pointer-events: none;
        }

        .top-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 5px;
        }

        .mode-label {
            color: var(--theme-color);
            font-family: 'Courier New', monospace;
            font-size: 1.2em;
            font-weight: bold;
            text-shadow: 0 0 5px var(--theme-glow);
        }

        .channel-info {
            color: var(--theme-color);
            font-family: 'Seven Segment', sans-serif;
            font-size: 1.4em;
            text-shadow: 0 0 5px var(--theme-glow);
        }

        .frequency {
            font-family: 'Seven Segment', sans-serif;
            font-size: 2.6em;
            color: var(--theme-color);
            text-shadow: 0 0 15px var(--theme-glow);
            letter-spacing: 1px;
            display: flex;
            align-items: baseline;
            justify-content: center;
            white-space: nowrap;
        }

        .mhz-unit {
            font-size: 0.4em;
            margin-left: 10px;
            font-family: 'Segoe UI', sans-serif;
            text-shadow: none;
            opacity: 0.8;
        }

        .smeter-label {
            font-size: 0.6em;
            color: #8b949e;
            margin-bottom: 5px;
            display: flex;
            justify-content: space-between;
            font-family: 'Courier New', monospace;
            font-weight: bold;
            padding: 0 2px;
        }

        .smeter-scale {
            display: flex;
            height: 8px;
            gap: 2px;
        }

        .smeter-segment {
            flex: 1;
            background: #161b22;
            border-radius: 1px;
        }

        .smeter-segment.on-theme { 
            background: var(--theme-color); 
            box-shadow: 0 0 5px var(--theme-color); 
        }

        .smeter-segment.on-red { 
            background: var(--led-red); 
            box-shadow: 0 0 5px var(--led-red); 
        }

        .waterfall-container {
            background: #050a05;
            border-radius: 5px;
            margin-bottom: 20px;
            border: 2px solid #101418;
            height: 50px;
            position: relative;
            overflow: hidden;
            box-shadow: inset 0 0 10px rgba(0,0,0,0.8);
        }

        #waterfall-canvas {
            width: 100%;
            height: 100%;
            display: block;
        }

        .middle-controls-row {
            display: flex;
            gap: 15px;
            margin-bottom: 25px;
            align-items: stretch;
        }

        .chan-lcd {
            flex: 1;
            background: var(--lcd-bg);
            border-radius: 12px;
            border: 3px solid #101418;
            box-shadow: inset 0 0 15px rgba(0,0,0,1);
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            position: relative;
            min-width: 120px;
        }

        .chan-lcd-label {
            position: absolute;
            top: 5px;
            left: 10px;
            font-size: 0.55em;
            color: #484f58;
            font-weight: 800;
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        #big-chan-num {
            font-family: 'Seven Segment', sans-serif;
            font-size: 3.8em;
            color: var(--theme-color);
            text-shadow: 0 0 15px var(--theme-glow);
            line-height: 1;
            margin-top: 5px;
        }

        .step-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 8px;
            flex: 1.4;
        }

        .step-grid button {
            padding: 8px 2px !important;
            font-size: 0.75em !important;
            min-height: 35px;
        }

        .knobs-wrapper {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin-bottom: 25px;
        }

        .knob-container {
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .knob-housing {
            position: relative;
            width: 160px;
            height: 160px;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .knob-surround {
            position: absolute;
            top: 0; left: 0; width: 100%; height: 100%;
            pointer-events: none;
        }

        .tick {
            position: absolute;
            width: 2px;
            height: 6px;
            background: #30363d;
            left: 50%;
            margin-left: -1px;
            transform-origin: 50% 80px;
            top: 5px;
        }

        .tick.major {
            height: 10px;
            background: #484f58;
            width: 3px;
        }

        .knob-number {
            position: absolute;
            font-size: 0.7em;
            color: #58a6ff;
            width: 20px;
            height: 20px;
            text-align: center;
            line-height: 20px;
            font-weight: bold;
            transform: translate(-50%, -50%);
        }

        .knob-outer {
            width: 110px;
            height: 110px;
            background: linear-gradient(145deg, #2c3138, #181c21);
            border-radius: 50%;
            box-shadow: 5px 5px 15px #0a0c0f, -5px -5px 15px #2a3037, inset 0 0 10px rgba(0,0,0,0.5);
            display: flex;
            justify-content: center;
            align-items: center;
            cursor: pointer;
            position: relative;
            touch-action: none;
            border: 3px solid #1c2128;
            z-index: 2;
        }

        .knob-inner {
            width: 85px;
            height: 85px;
            background: #1c2128;
            border-radius: 50%;
            position: relative;
            transition: transform 0.1s;
            box-shadow: inset 2px 2px 5px rgba(255,255,255,0.05), inset -2px -2px 5px rgba(0,0,0,0.5);
        }

        .knob-indicator {
            position: absolute;
            top: 8px;
            left: 50%;
            transform: translateX(-50%);
            width: 6px;
            height: 12px;
            background: var(--accent-color);
            border-radius: 3px;
            box-shadow: 0 0 10px var(--accent-color);
        }

        .knob-outer.channel-knob .knob-indicator {
            background: #ffaa00;
            box-shadow: 0 0 10px #ffaa00;
        }

        .knob-label {
            margin-top: 5px;
            font-size: 0.75em;
            color: #8b949e;
            text-transform: uppercase;
            letter-spacing: 2px;
            text-align: center;
            font-weight: bold;
        }

        .memory-section {
            border-top: 1px solid #30363d;
            padding-top: 20px;
        }

        .memory-title {
            font-size: 0.8em;
            color: var(--accent-color);
            margin-bottom: 10px;
            text-transform: uppercase;
            letter-spacing: 1px;
            text-align: left;
        }

        .memory-grid {
            display: grid;
            grid-template-columns: repeat(5, 1fr);
            gap: 8px;
        }

        button {
            background: #21262d;
            border: 1px solid #30363d;
            color: var(--text-color);
            padding: 8px 5px;
            border-radius: 8px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.1s;
            box-shadow: 0 4px 0 #101418;
            user-select: none;
            -webkit-tap-highlight-color: transparent;
        }

        button:active {
            transform: translateY(3px);
            box-shadow: 0 1px 0 #101418;
            background: #30363d;
        }

        button.plus { color: #58a6ff; }
        button.minus { color: #f85149; }
        
        button.mode {
            font-size: 0.8em;
            padding: 4px 2px !important;
            background: #161b22;
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 30px;
        }

        button.mode.active {
            border-color: var(--accent-color);
            color: var(--accent-color);
            box-shadow: 0 4px 0 var(--theme-glow);
        }

        button.mem { 
            font-size: 0.8em; 
            padding: 4px 6px !important; 
            background: #161b22;
            position: relative;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: space-between;
            min-height: 42px;
        }

        .mem-header {
            display: flex;
            justify-content: space-between;
            width: 100%;
            font-size: 0.75em;
            font-weight: bold;
            color: var(--accent-color);
            margin-bottom: 2px;
        }

        .mem-chan {
            color: #ffaa00;
        }

        .mem-freq {
            display: block;
            font-size: 0.9em;
            color: var(--theme-color);
            font-family: 'Seven Segment', sans-serif;
            letter-spacing: 0.5px;
        }

        button.mem.active {
            border-color: var(--accent-color);
            color: var(--accent-color);
            box-shadow: 0 4px 0 var(--theme-glow);
        }

        .footer {
            margin-top: 25px;
            font-size: 0.65em;
            color: #484f58;
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        button.mem::after {
            content: "";
            position: absolute;
            bottom: 2px; left: 10%; width: 0%; height: 2px;
            background: var(--led-red);
            transition: width 1s linear;
        }

        button.mem:active::after { 
            width: 80%; 
        }
    </style>
</head>
<body>
    <div class="radio-unit">
        <div class="header">
            <div class="header-top">
                <div class="brand">
                    <span>President Grant Digital</span>
                    <span class="settings-btn" onclick="toggleConfig()">⚙️</span>
                </div>
            </div>
            <div class="header-bottom">
                <div id="conn-status" class="conn-status offline">OFFLINE</div>
                <div class="status-indicators">
                    <div id="led-rx" class="led active-rx"><div class="led-dot"></div>RX</div>
                    <div id="led-tx" class="led"><div class="led-dot"></div>TX</div>
                </div>
            </div>
        </div>

        <div id="config-panel" class="config-panel">
            <div style="font-weight: bold; margin-bottom: 10px; color: var(--accent-color);">Advanced Settings</div>
            <div class="config-grid">
                <div class="config-item">
                    <label>Min Freq (Hz)</label>
                    <input type="number" id="cfg-min" onchange="saveConfig('min', this.value)">
                </div>
                <div class="config-item">
                    <label>Max Freq (Hz)</label>
                    <input type="number" id="cfg-max" onchange="saveConfig('max', this.value)">
                </div>
                <div class="config-item">
                    <label>FI AM (Hz)</label>
                    <input type="number" id="cfg-fi-am" onchange="saveConfig('fi_am', this.value)">
                </div>
                <div class="config-item">
                    <label>FI USB (Hz)</label>
                    <input type="number" id="cfg-fi-usb" onchange="saveConfig('fi_usb', this.value)">
                </div>
                <div class="config-item">
                    <label>FI LSB (Hz)</label>
                    <input type="number" id="cfg-fi-lsb" onchange="saveConfig('fi_lsb', this.value)">
                </div>
                <div class="config-item">
                    <label>VFO Adj</label>
                    <input type="number" id="cfg-vfo" onchange="saveConfig('vfo_adj', this.value)">
                </div>
                <div class="config-item">
                    <label>SMeter Adj</label>
                    <input type="number" id="cfg-sm" onchange="saveConfig('sm_adj', this.value)">
                </div>
                <div class="config-item">
                    <label>Clarifier Center</label>
                    <input type="number" id="cfg-cl" onchange="saveConfig('cl_center', this.value)">
                </div>
                <div class="config-item">
                    <label>Step Inc (Hz)</label>
                    <input type="number" id="cfg-step" onchange="saveConfig('step', this.value)">
                </div>
                <div class="config-item">
                    <label>Screen Style</label>
                    <select id="cfg-theme" onchange="applyTheme(this.value); saveConfig('theme', this.value)">
                        <option value="0">Classic Green</option>
                        <option value="1">Vintage Amber</option>
                        <option value="2">Deep Blue</option>
                        <option value="3">Arctic White</option>
                        <option value="4">Emergency Red</option>
                    </select>
                </div>
                <div class="config-item">
                    <label>S-Meter Display</label>
                    <div class="toggle-group">
                        <button id="btn-sm-on" class="toggle-btn" onclick="setShowSMeter(1)">ON</button>
                        <button id="btn-sm-off" class="toggle-btn" onclick="setShowSMeter(0)">OFF</button>
                    </div>
                </div>
                <div class="config-item">
                    <label>Waterfall Display</label>
                    <div class="toggle-group">
                        <button id="btn-wf-on" class="toggle-btn" onclick="setShowWaterfall(1)">ON</button>
                        <button id="btn-wf-off" class="toggle-btn" onclick="setShowWaterfall(0)">OFF</button>
                    </div>
                </div>
            </div>
            <div class="config-actions">
                <button onclick="resetMemories()" style="background: var(--led-red); padding: 8px 15px; font-size: 0.9em; box-shadow: 0 4px 0 #8e1c1c;">Reset Memories</button>
            </div>
        </div>

        <div class="display-screen">
            <div class="top-row">
                <div id="mode" class="mode-label">USB</div>
                <div class="channel-info"><span id="band" style="font-size: 0.8em; margin-right: 10px; opacity: 0.7;">11m</span><span id="channel">CH 19</span></div>
            </div>
            <div class="frequency"><span id="freq">27.555.000</span><span class="mhz-unit">MHz</span></div>
            
            <div id="smeter-block" style="margin-top: 15px;">
                <div id="smeter-text" style="font-size: 0.7em; color: var(--theme-color); font-family: monospace; text-align: right; margin-bottom: 2px; height: 1em;">S0</div>
                <div class="smeter-label">
                    <span>S</span>
                    <span>1</span><span>.</span><span>3</span><span>.</span><span>5</span><span>.</span><span>7</span><span>.</span><span>9</span><span>.</span><span>+10</span><span>.</span><span>.</span><span>+30</span>
                </div>
                <div id="smeter-scale" class="smeter-scale"></div>
            </div>
        </div>

        <div class="modes-section" style="margin-bottom: 20px;">
            <div class="memory-grid">
                <button class="mode" id="mode0" onclick="changeMode(0)">CW</button>
                <button class="mode" id="mode1" onclick="changeMode(1)">AM</button>
                <button class="mode" id="mode2" onclick="changeMode(2)">FM</button>
                <button class="mode" id="mode3" onclick="changeMode(3)">USB</button>
                <button class="mode" id="mode4" onclick="changeMode(4)">LSB</button>
            </div>
        </div>

        <div class="waterfall-container">
            <canvas id="waterfall-canvas"></canvas>
        </div>

        <div class="middle-controls-row">
            <div class="chan-lcd">
                <div class="chan-lcd-label">Channel</div>
                <div id="big-chan-num">19</div>
            </div>
            <div class="step-grid">
                <button class="plus" onclick="changeFreq(10000)">+10k</button>
                <button class="plus" onclick="changeFreq(1000)">+1k</button>
                <button class="plus" onclick="changeFreq(100)">+100</button>
                <button class="minus" onclick="changeFreq(-10000)">-10k</button>
                <button class="minus" onclick="changeFreq(-1000)">-1k</button>
                <button class="minus" onclick="changeFreq(-100)">-100</button>
            </div>
        </div>

        <div class="knobs-wrapper">
            <div class="knob-container">
                <div class="knob-housing">
                    <div id="knob-surround-tuning" class="knob-surround"></div>
                    <div id="knob" class="knob-outer">
                        <div id="knob-inner" class="knob-inner">
                            <div class="knob-indicator"></div>
                        </div>
                    </div>
                </div>
                <div class="knob-label">Tuning</div>
            </div>
            <div class="knob-container">
                <div class="knob-housing">
                    <div id="knob-surround-channels" class="knob-surround"></div>
                    <div id="knob-chan" class="knob-outer channel-knob">
                        <div id="chan-knob-inner" class="knob-inner">
                            <div class="knob-indicator"></div>
                        </div>
                    </div>
                </div>
                <div class="knob-label">Channels</div>
            </div>
        </div>

        <div class="memory-section">
            <div class="memory-title">Memories (Press and hold to save)</div>
            <div class="memory-grid">
                <button class="mem" id="mem0" onmousedown="startMem(0)" onmouseup="endMem(0)" ontouchstart="startMem(0)" ontouchend="endMem(0)">
                    <div class="mem-header"><span>M1</span><span class="mem-chan" id="mc0">--</span></div>
                    <div class="mem-freq" id="mf0">--</div>
                </button>
                <button class="mem" id="mem1" onmousedown="startMem(1)" onmouseup="endMem(1)" ontouchstart="startMem(1)" ontouchend="endMem(1)">
                    <div class="mem-header"><span>M2</span><span class="mem-chan" id="mc1">--</span></div>
                    <div class="mem-freq" id="mf1">--</div>
                </button>
                <button class="mem" id="mem2" onmousedown="startMem(2)" onmouseup="endMem(2)" ontouchstart="startMem(2)" ontouchend="endMem(2)">
                    <div class="mem-header"><span>M3</span><span class="mem-chan" id="mc2">--</span></div>
                    <div class="mem-freq" id="mf2">--</div>
                </button>
                <button class="mem" id="mem3" onmousedown="startMem(3)" onmouseup="endMem(3)" ontouchstart="startMem(3)" ontouchend="endMem(3)">
                    <div class="mem-header"><span>M4</span><span class="mem-chan" id="mc3">--</span></div>
                    <div class="mem-freq" id="mf3">--</div>
                </button>
                <button class="mem" id="mem4" onmousedown="startMem(4)" onmouseup="endMem(4)" ontouchstart="startMem(4)" ontouchend="endMem(4)">
                    <div class="mem-header"><span>M5</span><span class="mem-chan" id="mc4">--</span></div>
                    <div class="mem-freq" id="mf4">--</div>
                </button>
            </div>
        </div>

        <div class="footer">
            ESP32 )=====" WEB_VERSION R"=====( &bull; Remote Controller
        </div>
    </div>

    <script>
        let currentFreq = 0;
        let memories = [0,0,0,0,0];
        let memTimer = null;
        let configLoaded = false;
        let isWaiting = false; 
        let isDragging = false; 
        let chanDragging = false; 
        let lastClickedMem = -1;
        let lastSentFreq = 0;
        let freqThrottleTimer = null;
        const smeterScale = document.getElementById('smeter-scale');
        
        const themes = [
            { color: '#2ecc71', glow: 'rgba(46, 204, 113, 0.7)' },
            { color: '#ffb300', glow: 'rgba(255, 179, 0, 0.7)' },
            { color: '#3498db', glow: 'rgba(52, 152, 219, 0.7)' },
            { color: '#ecf0f1', glow: 'rgba(236, 240, 241, 0.7)' },
            { color: '#e74c3c', glow: 'rgba(231, 76, 60, 0.7)' }
        ];

        for(let i=0; i<20; i++) {
            const seg = document.createElement('div');
            seg.className = 'smeter-segment';
            smeterScale.appendChild(seg);
        }

        // Initialize knob markers
        function initKnobMarkers() {
            const tuningSurround = document.getElementById('knob-surround-tuning');
            const chanSurround = document.getElementById('knob-surround-channels');
            const center = 80; // center of 160x160 housing
            
            // Tuning markers (ticks every 15 deg)
            for (let i = 0; i < 360; i += 15) {
                const tick = document.createElement('div');
                tick.className = 'tick' + (i % 45 === 0 ? ' major' : '');
                tick.style.transform = `rotate(${i}deg)`;
                tuningSurround.appendChild(tick);
            }
            
            // Channel markers (1 to 40)
            for (let i = 1; i <= 40; i++) {
                const angle = (i - 1) * (360 / 40);
                const tick = document.createElement('div');
                tick.className = 'tick major';
                tick.style.transform = `rotate(${angle}deg)`;
                chanSurround.appendChild(tick);
                
                if (i % 5 === 0 || i === 1) {
                    const num = document.createElement('div');
                    num.className = 'knob-number';
                    num.innerText = i;
                    // Position upright with trig
                    const rad = (angle - 90) * Math.PI / 180;
                    const r = 70; // radius for numbers
                    num.style.left = (center + Math.cos(rad) * r) + 'px';
                    num.style.top = (center + Math.sin(rad) * r) + 'px';
                    chanSurround.appendChild(num);
                }
            }
        }
        initKnobMarkers();

        // Waterfall Logic
        const wfCanvas = document.getElementById('waterfall-canvas');
        const wfCtx = wfCanvas.getContext('2d', { willReadFrequently: true });
        function initWF() {
            wfCanvas.width = wfCanvas.offsetWidth;
            wfCanvas.height = wfCanvas.offsetHeight;
            wfCtx.fillStyle = '#050a05';
            wfCtx.fillRect(0, 0, wfCanvas.width, wfCanvas.height);
        }
        window.addEventListener('resize', initWF);
        initWF();

        function updateConnStatus(online) {
            const status = document.getElementById('conn-status');
            if(online) {
                status.innerText = "ONLINE";
                status.classList.add('online');
                status.classList.remove('offline');
            } else {
                status.innerText = "OFFLINE";
                status.classList.add('offline');
                status.classList.remove('online');
            }
        }

        function toggleSMeter(show) {
            const smBlock = document.getElementById('smeter-block');
            const wfBlock = document.querySelector('.waterfall-container');
            const display = show ? 'block' : 'none';
            smBlock.style.display = display;
            wfBlock.style.display = display;
        }

        function addWFRow(val) {
            const w = wfCanvas.width;
            const h = wfCanvas.height;
            const imgData = wfCtx.getImageData(0, 0, w, h - 1);
            wfCtx.putImageData(imgData, 0, 1);
            
            const level = val / 1024;
            const themeIdx = document.getElementById('cfg-theme').value;
            const color = themes[themeIdx].color;
            
            wfCtx.fillStyle = level > 0.1 ? color : '#050a05';
            wfCtx.globalAlpha = level;
            wfCtx.fillRect(0, 0, w, 1);
            wfCtx.globalAlpha = 1.0;
        }

        function applyTheme(idx) {
            const theme = themes[idx] || themes[0];
            document.documentElement.style.setProperty('--theme-color', theme.color);
            document.documentElement.style.setProperty('--theme-glow', theme.glow);
            document.getElementById('cfg-theme').value = idx;
        }

        function toggleConfig() {
            const panel = document.getElementById('config-panel');
            panel.style.display = (panel.style.display === 'block') ? 'none' : 'block';
        }

        function saveConfig(param, value) {
            fetch(`/updateconfig?${param}=${value}`).catch(err => console.error("Save failed"));
        }

        function resetMemories() {
            if (confirm("Are you sure you want to reset all 5 memories?")) {
                fetch('/resetmem').then(() => { alert("Done!"); updateStatus(); });
            }
        }

        function setShowSMeter(val) {
            toggleSMeter(val == 1);
            saveConfig('show_sm', val);
            document.getElementById('btn-sm-on').classList.toggle('active', val == 1);
            document.getElementById('btn-sm-off').classList.toggle('active', val == 0);
        }

        function setShowWaterfall(val) {
            const wfBlock = document.querySelector('.waterfall-container');
            wfBlock.style.display = (val == 1) ? 'block' : 'none';
            saveConfig('show_wf', val);
            document.getElementById('btn-wf-on').classList.toggle('active', val == 1);
            document.getElementById('btn-wf-off').classList.toggle('active', val == 0);
        }

        function updateLocalChannelInfo(f) {
            const chanDiv = document.getElementById('channel');
            const baseFreq = 26965000;
            if (f >= 26965000 && f <= 27405000) {
                let ch = Math.floor((f - baseFreq + 5000) / 10000) + 1;
                chanDiv.innerText = "CH " + ch;
                chanDiv.style.opacity = "1";
            } else {
                chanDiv.innerText = "OUT";
                chanDiv.style.opacity = "0.5";
            }
        }

        function updateStatus() {
            if (isWaiting || isDragging || chanDragging) return;
            
            isWaiting = true;
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    isWaiting = false;
                    updateConnStatus(true);
                    
                    // Sync knobs if changed externally or on first load
                    if (Math.abs(currentFreq - data.freq) > 50) {
                        syncKnobs(data.freq, data.channel);
                    }

                    currentFreq = data.freq;
                    memories = data.memories;
                    document.getElementById('freq').innerText = formatFreq(data.freq);
                    document.getElementById('mode').innerText = data.mode;
                    const chanDiv = document.getElementById('channel');
                    const bigChanDiv = document.getElementById('big-chan-num');
                    const bandSpan = document.getElementById('band');
                    if (data.channel !== -1) {
                        const chanTxt = data.channel + (data.bis ? "b" : "");
                        chanDiv.innerText = "CH " + chanTxt;
                        bigChanDiv.innerText = chanTxt;
                        chanDiv.style.opacity = "1";
                    } else {
                        chanDiv.innerText = "OUT";
                        bigChanDiv.innerText = "--";
                        chanDiv.style.opacity = "0.5";
                    }
                    bandSpan.innerText = data.band;
                    if (!configLoaded && data.conf) {
                        document.getElementById('cfg-min').value = data.conf.min;
                        document.getElementById('cfg-max').value = data.conf.max;
                        document.getElementById('cfg-fi-am').value = data.conf.fi_am;
                        document.getElementById('cfg-fi-usb').value = data.conf.fi_usb;
                        document.getElementById('cfg-fi-lsb').value = data.conf.fi_lsb;
                        document.getElementById('cfg-vfo').value = data.conf.vfo_adj;
                        document.getElementById('cfg-sm').value = data.conf.sm_adj;
                        document.getElementById('cfg-cl').value = data.conf.cl_center;
                        document.getElementById('cfg-step').value = data.conf.step;
                        applyTheme(data.conf.theme);
                        
                        document.getElementById('btn-sm-on').classList.toggle('active', data.conf.show_sm == 1);
                        document.getElementById('btn-sm-off').classList.toggle('active', data.conf.show_sm == 0);
                        toggleSMeter(data.conf.show_sm == 1);

                        document.getElementById('btn-wf-on').classList.toggle('active', data.conf.show_wf == 1);
                        document.getElementById('btn-wf-off').classList.toggle('active', data.conf.show_wf == 0);
                        document.querySelector('.waterfall-container').style.display = (data.conf.show_wf == 1) ? 'block' : 'none';

                        configLoaded = true;
                    }
                    const ledRx = document.getElementById('led-rx');
                    const ledTx = document.getElementById('led-tx');
                    const freqDisplay = document.getElementById('freq');
                    const modeDisplay = document.getElementById('mode');
                    if (data.tx) {
                        ledTx.classList.add('active-tx'); ledRx.classList.remove('active-rx');
                        freqDisplay.style.color = 'var(--led-red)'; modeDisplay.style.color = 'var(--led-red)';
                    } else {
                        ledTx.classList.remove('active-tx'); ledRx.classList.add('active-rx');
                        freqDisplay.style.color = 'var(--theme-color)'; modeDisplay.style.color = 'var(--theme-color)';
                    }
                    updateSMeter(data.smeter);
                    updateMemories();
                    
                    for(let i=0; i<5; i++) {
                        const mBtn = document.getElementById('mode' + i);
                        if(i === data.modeIdx) mBtn.classList.add('active');
                        else mBtn.classList.remove('active');
                    }
                })
                .catch(e => { 
                    console.error("Status error", e);
                    isWaiting = false; 
                    updateConnStatus(false);
                });
        }

        function syncKnobs(f, ch) {
            // Sync Channel Knob using real channel number from ESP32
            if (ch !== -1) {
                chanRotation = (ch - 1) * 9;
                chanLastTrigger = chanRotation;
                chanKnobInner.style.transform = `rotate(${chanRotation}deg)`;
            }
            // Sync Tuning Knob (Visual only, 15 deg per 100Hz)
            currentRotation = (f / 100) * 15; 
            lastTriggerAngle = currentRotation;
            knobInner.style.transform = `rotate(${currentRotation}deg)`;
        }

        const cibiChannels = [1, 2, 3, 3, 4, 5, 6, 7, 7, 8, 9, 10, 11, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40];
        const cibiBis = [false, false, false, true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false];

        function updateMemories() {
            let activeIdx = -1;
            if (lastClickedMem !== -1 && Math.abs(memories[lastClickedMem] - currentFreq) < 20) activeIdx = lastClickedMem;
            else {
                for (let i = 0; i < 5; i++) {
                    if (Math.abs(memories[i] - currentFreq) < 20) { activeIdx = i; break; }
                }
            }
            for(let i=0; i<5; i++) {
                const btn = document.getElementById('mem' + i);
                const freq = memories[i];
                
                // Update Frequency with format 27.270.00
                const freqEl = document.getElementById('mf' + i);
                if (freq === 0) {
                    freqEl.innerText = "--.---.--";
                } else {
                    let s = freq.toString();
                    while (s.length < 8) s = '0' + s;
                    freqEl.innerText = s.substring(0, 2) + '.' + s.substring(2, 5) + '.' + s.substring(5, 7);
                }

                // Update Channel (Correct CB Logic)
                const chanEl = document.getElementById('mc' + i);
                const freqDakhz = Math.floor(freq / 10000);
                const lowDakhz = 2561;
                const highDakhz = 2875;
                
                if (freqDakhz >= lowDakhz && freqDakhz <= highDakhz) {
                    const index = (freqDakhz - lowDakhz) % 45;
                    const ch = cibiChannels[index];
                    const bis = cibiBis[index];
                    chanEl.innerText = ch + (bis ? "b" : "");
                } else {
                    chanEl.innerText = "--";
                }

                if(i === activeIdx) btn.classList.add('active'); else btn.classList.remove('active');
            }
        }

        function startMem(idx) {
            memTimer = setTimeout(() => {
                isWaiting = true;
                fetch(`/setmem?index=${idx}&freq=${currentFreq}`).then(() => {
                    memTimer = null; isWaiting = false; lastClickedMem = idx;
                    if (navigator.vibrate) navigator.vibrate([30, 30, 30]);
                    updateStatus();
                }).catch(() => { isWaiting = false; memTimer = null; });
            }, 700);
        }

        function endMem(idx) {
            if(memTimer) {
                clearTimeout(memTimer); memTimer = null; isWaiting = true;
                fetch(`/set?freq=${memories[idx]}`).then(() => {
                    isWaiting = false; lastClickedMem = idx; updateStatus();
                    if (navigator.vibrate) navigator.vibrate(10);
                }).catch(() => { isWaiting = false; });
            }
        }

        function updateSMeter(val) {
            const segments = document.querySelectorAll('.smeter-segment');
            const level = Math.floor((val / 1024) * 20);
            segments.forEach((seg, i) => {
                seg.className = 'smeter-segment';
                if(i < level) seg.classList.add(i < 14 ? 'on-theme' : 'on-red');
            });
            
            const smText = document.getElementById('smeter-text');
            if (level <= 1) smText.innerText = "S0";
            else if (level <= 14) smText.innerText = "S" + Math.floor(level/1.5);
            else if (level <= 17) smText.innerText = "S9 +10dB";
            else smText.innerText = "S9 +30dB";
            
            addWFRow(val);
        }

        function formatFreq(f) {
            let s = f.toString();
            while (s.length < 8) s = '0' + s;
            return s.substring(0, 2) + '.' + s.substring(2, 5) + '.' + s.substring(5, 8);
        }

        function sendFreqUpdate() {
            if (currentFreq === lastSentFreq) return;
            lastSentFreq = currentFreq;
            fetch('/set?freq=' + currentFreq).catch(() => {});
        }

        function changeFreq(delta) {
            lastClickedMem = -1; currentFreq += delta;
            document.getElementById('freq').innerText = formatFreq(currentFreq);
            updateLocalChannelInfo(currentFreq);
            
            if (freqThrottleTimer) clearTimeout(freqThrottleTimer);
            freqThrottleTimer = setTimeout(sendFreqUpdate, 50); // 50ms throttle
        }

        function changeMode(m) {
            fetch(`/set?mode=${m}`).then(() => updateStatus());
            if (navigator.vibrate) navigator.vibrate(20);
        }

        setInterval(updateStatus, 200); 
        updateStatus();

        const knob = document.getElementById('knob');
        const knobInner = document.getElementById('knob-inner');
        const chanKnob = document.getElementById('knob-chan');
        const chanKnobInner = document.getElementById('chan-knob-inner');
        let startAngle = 0, currentRotation = 0, lastTriggerAngle = 0;
        let chanStartAngle = 0, chanRotation = 0, chanLastTrigger = 0;

        function getAngleEl(el, x, y) {
            const rect = el.getBoundingClientRect();
            return Math.atan2(y - (rect.top + rect.height / 2), x - (rect.left + rect.width / 2)) * 180 / Math.PI;
        }

        function handleStart(e) { isDragging = true; const p = e.touches ? e.touches[0] : e; startAngle = getAngleEl(this, p.clientX, p.clientY) - currentRotation; }
        function handleMove(e) {
            if (!isDragging) return; e.preventDefault(); const p = e.touches ? e.touches[0] : e;
            const angle = getAngleEl(knob, p.clientX, p.clientY); currentRotation = angle - startAngle;
            const snap = Math.round(currentRotation / 15) * 15;
            knobInner.style.transform = `rotate(${snap}deg)`;
            const diff = snap - lastTriggerAngle;
            if (Math.abs(diff) >= 15) { 
                const steps = Math.trunc(diff / 15); 
                changeFreq(steps * 100); 
                lastTriggerAngle += steps * 15; 
                if (navigator.vibrate) navigator.vibrate(5); 
            }
        }
        function handleEnd() { isDragging = false; }
        
        knob.addEventListener('mousedown', handleStart); window.addEventListener('mousemove', handleMove); window.addEventListener('mouseup', handleEnd);
        knob.addEventListener('touchstart', handleStart); window.addEventListener('touchmove', handleMove, { passive: false }); window.addEventListener('touchend', handleEnd);

        function handleChanStart(e) { chanDragging = true; const p = e.touches ? e.touches[0] : e; chanStartAngle = getAngleEl(this, p.clientX, p.clientY) - chanRotation; }
        function handleChanMove(e) {
            if (!chanDragging) return; e.preventDefault(); const p = e.touches ? e.touches[0] : e;
            const angle = getAngleEl(chanKnob, p.clientX, p.clientY); chanRotation = angle - chanStartAngle;
            const snap = Math.round(chanRotation / 9) * 9;
            chanKnobInner.style.transform = `rotate(${snap}deg)`;
            const diff = snap - chanLastTrigger;
            if (Math.abs(diff) >= 9) { 
                const steps = Math.trunc(diff / 9); 
                changeFreq(steps * 10000); 
                chanLastTrigger += steps * 9; 
                if (navigator.vibrate) navigator.vibrate(15); 
            }
        }
        function handleChanEnd() { chanDragging = false; }
        
        chanKnob.addEventListener('mousedown', handleChanStart); window.addEventListener('mousemove', handleChanMove); window.addEventListener('mouseup', handleChanEnd);
        chanKnob.addEventListener('touchstart', handleChanStart); window.addEventListener('touchmove', handleChanMove, { passive: false }); window.addEventListener('touchend', handleChanEnd);
    </script>
</body>
</html>
)=====";

#endif
