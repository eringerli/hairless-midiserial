-------------------------------------------------------------------------------
-- States: page, colors, brightness, etc
-------------------------------------------------------------------------------

page = {
  ["currentPage"] = 1,
  ["maxPages"] = 3
}

colors = {
  ["red"] = {0,127},
  ["green"] = {45,127},
  ["blue"] = {84,127},
  ["magenta"] = {105,127},
  ["yellow"] = {22,127},
  ["cyan"] = {64,127},
  ["white"] = {0,0},
  ["orange"] = {12,127},
  ["goldyellow"] = {22,127},
  ["purple"] = {96,127},
  ["salmon"] = {4,64},
  ["lightgreen"] = {36,64},
  ["lightblue"] = {86,64},
  ["lightyellow"] = {24,52},
  ["lightmagenta"] = {104,52},
  ["violet"] = {96,118},
  ["lightcyan"] = {64,52}
}

padColors = {
  {
    [1] = {colors["red"]},
    [2] = {colors["green"]},
    [3] = {colors["blue"]},
    [4] = {colors["magenta"]},
    [5] = {colors["yellow"]},
    [6] = {colors["cyan"]},
    [7] = {colors["white"]},
    [8] = {colors["lightgreen"]},
    [9] = {colors["blue"],colors["violet"],colors["blue"],colors["red"],colors["blue"],colors["violet"]},
    [10] = {colors["green"],colors["lightgreen"],colors["goldyellow"],colors["lightyellow"],colors["orange"]},
    [11] = {colors["red"],colors["violet"],colors["red"],colors["magenta"]},
    [12] = {colors["violet"],colors["lightgreen"],colors["lightblue"],colors["salmon"]},
    [13] = {colors["goldyellow"],colors["lightmagenta"],colors["lightgreen"]},
    [14] = {colors["red"],colors["blue"],colors["green"],colors["yellow"],colors["white"]},
    [15] = {colors["blue"],colors["lightgreen"],colors["violet"]},
    [16] = {colors["lightgreen"],colors["red"],colors["lightblue"]}
  },
  {
    [1] = {colors["lightblue"]},
    [2] = {colors["lightblue"]},
    [3] = {colors["lightblue"]},
    [4] = {},--colors["white"]},
    [5] = {colors["lightblue"]},
    [6] = {colors["lightblue"]},
    [7] = {colors["lightblue"]},
    [8] = {colors["lightgreen"]},
    [9] = {colors["lightblue"]},
    [10] = {colors["lightblue"]},
    [11] = { -- rainbow ;)
      colors["red"],colors["orange"],colors["goldyellow"],colors["green"],colors["violet"],colors["blue"],
      colors["blue"],colors["violet"],colors["green"],colors["goldyellow"],colors["orange"],colors["red"]
    },
    [12] = {},--colors["white"]},
    [13] = {},--colors["white"]},
    [14] = {},--colors["white"]},
    [15] = {},--colors["white"]},
    [16] = {}--colors["white"]}
  },
  {
    [1] = {colors["lightgreen"]},
    [2] = {colors["lightgreen"]},
    [3] = {colors["lightgreen"]},
    [4] = {},--colors["white"]},
    [5] = {colors["lightgreen"]},
    [6] = {colors["lightgreen"]},
    [7] = {colors["lightgreen"]},
    [8] = {},--colors["white"]},
    [9] = {},--colors["white"]},
    [10] = {},--colors["white"]},
    [11] = {},--colors["white"]},
    [12] = {},--colors["white"]},
    [13] = {colors["salmon"]},
    [14] = {},--colors["white"]},
    [15] = {},--colors["white"]},
    [16] = {colors["red"]}
  }
}

padColorState = {
  {},
  {},
  {},
}

padLastColor={{}}

padBrightnessState = {
  {10,10,10,10, 10,10,10,10, 10,10,10,10, 10,10,10,10},
  {10,10,10,10, 10,10,10,10, 10,10,10,10, 10,10,10,10},
  {10,10,10,10, 10,10,10,10, 10,10,10,10, 10,10,10,10}
}

specialKeyBrightness = {10,10,10,10, 10,10,10,10, 10,10,10}

padLastBrightness={}

specialKeyLastBrightness = {}

padColorIndex = 0
padStartControllerNumberOffset = 9

padMinBrightness = 10
padMiddleBrightness = 50
padMaxBrightness = 100

-------------------------------------------------------------------------------
-- Left -> Right
-------------------------------------------------------------------------------

function noteOn(channel, note, velocity)
  print("noteOn: ",channel, note,velocity)
end

function noteOff(channel, note, velocity)
  print("noteOff: ",channel, note,velocity)
end

function keyPressure(channel, note, pressure)
  print("keyPressure: ",channel, note,pressure)
end

controller7Value = 0;
controller8Value = 0;
deckSelectPressedButtons = 0;
deckSelectAutoLatch = false;

function controller(channel, controller, value)
-- encoder -> select page
  if(controller==41) then
    if(value == 127) then
      page.currentPage=decrementRollOver(page.currentPage, 1, page.maxPages)
    else
      page.currentPage=incrementRollOver(page.currentPage, 1, page.maxPages)
    end
    padLastBrightness = nil
    padLastBrightness = {}

    displayNumber(sendMessageLeft, page.currentPage)
    colorFlow()

    return
  end

  if(controller==2) then
    setMinBrightness(value)
    return
  end

  if(controller==3) then
    setMaxBrightness(value)
    return
  end

  if(controller==4) then
    if(value>=20) then
      setTimeout(value)
    end
  end

  if(controller==5) then
    if(value>=2) then
      padColorsFlowSteps=value/2
    end
  end

--  if(controller==4) then
--    sendHSB(sendMessageLeft, 10, nil,nil,value)
--  end


--  if(controller==5) then
--    if(value>1) then
--      setTimeout(value*5)
--    end
--  end

--  if(controller==6) then
--    local padBrightnessStateForPage = padBrightnessState[page.currentPage]

--    for k=16,#padBrightnessStateForPage do
--      padBrightnessStateForPage[k] = value
--    end
--    sendPadBrightness()
--  end

  -- second fader: 5 NoteOn for strobo (auto, off, low, middle, fast)
  if(controller == 7) then
    local tone = math.floor(value / (128/5))
    if(controller7Value~=tone) then
      sendNoteOn( sendMessageRight, 114+tone, 127)
      sendNoteOn( sendMessageRight, 114+tone, 0)
    end
    controller7Value=tone

    return
  end
  -- third fader: 4 NoteOn for mood (auto, low, middle, high)
  if(controller == 8) then
    local tone = math.floor(value / (128/4))
    if(controller8Value~=tone) then
      sendNoteOn( sendMessageRight, 104+tone, 127)
      sendNoteOn( sendMessageRight, 104+tone, 0)
    end
    controller8Value=tone

    return
  end

  -- bottom row: deck select. press two buttons simultaniously to enable auto
  if(controller>=37 and controller<=40) then
    if(value == 127) then
      deckSelectPressedButtons = deckSelectPressedButtons+1
    end

    if(value == 0) then
      deckSelectPressedButtons = deckSelectPressedButtons-1

      if(deckSelectPressedButtons>=1) then
        deckSelectAutoLatch = true
      end

      if(deckSelectPressedButtons==0 and deckSelectAutoLatch == true) then
        sendNoteOn(sendMessageRight, 108, 127)
        sendNoteOn(sendMessageRight, 108, 0)
        deckSelectAutoLatch = false
      else
        if(deckSelectAutoLatch==false) then
          sendNoteOn(sendMessageRight, 108+(controller-36), 127)
          sendNoteOn(sendMessageRight, 108+(controller-36), 0)
        end
      end

    end
    return
  end

  -- another hack: reset all light when the last button is pressed
  if(controller == 86 and value == 0) then
    for index=1,16 do
      padBrightnessState[page.currentPage][index]=padMinBrightness
    end
    return
  end


  -- fallback: send everything through

  -- all the analog knobs, not paged
  if((controller>=2 and controller<=9) or (controller>=43 and controller<=50)) then
    sendController(sendMessageRight, controller, value)
  end

  -- all the pads, paged
  if((controller>=10 and controller<=25) or (controller>=50 and controller<=65) or (controller==78)) then
    local note = controller-padStartControllerNumberOffset+ (page.currentPage-1)*16
    sendNoteOn( sendMessageRight,note,value)

    -- hack: simulate feedback for page 2 (custom DMX) from rekordbox
    if(page.currentPage==2 and value == 0) then
      noteOnRight(channel, note, 127)
    end
    return
  end

  -- the bottom row, not paged
  if((controller>=37 and controller<=40) or (controller>=74 and controller<=77)) then
    sendController(sendMessageRight, controller, value)
  end
end

function programm(channel, programChange)
  print("programm: ",channel, programChange)
end

function channelPressure(channel, pressure)
  print("channelPressure: ",channel, pressure)
end

function pitchBend(channel, valueLow, valueHigh)
  print("pitchBend: ",channel, valueLow,valueHigh)
end

function parseMidiMessage(x)
  local tag = x[1]&TAG_MASK
  local channel = (x[1] & CHANNEL_MASK)+1

  if (tag == TAG_NOTE_ON) then
    noteOn(channel, x[2], x[3])
  end
  if (tag == TAG_NOTE_OFF) then
    noteOff(channel, x[2], x[3])
  end
  if (tag == TAG_KEY_PRESSURE) then
    keyPressure(channel, x[2], x[3])
  end
  if (tag == TAG_CONTROLLER) then
    controller(channel, x[2], x[3])
  end
  if (tag == TAG_PROGRAM) then
    programm(channel, x[2])
  end
  if (tag == TAG_CHANNEL_PRESSURE) then
    channelPressure(channel, x[2])
  end
  if (tag == TAG_PITCH_BEND) then
    pitchBend(channel, x[2], x[3])
  end
end

-------------------------------------------------------------------------------
-- Right -> Left
-------------------------------------------------------------------------------

autoDeckSelect = true
deckSelected = 1

function controllerRight(channel, controller, value)

end

function noteOnRight(channel, note, velocity)

  -- paged pads
  if(note <= (page.maxPages*16)) then
    local pageNum = 1
    for i=1,#padBrightnessState do
      if((note - 16) > 0) then
        note = note - 16
      else
        pageNum = i
        break
      end
    end

    if(padBrightnessState[pageNum][note] == padMinBrightness) then
--  if(velocity == 127) then
      padBrightnessState[pageNum][note] = padMaxBrightness
    else
      padBrightnessState[pageNum][note] = padMinBrightness
    end

    return
  end

  -- deck selector
  if(note >= 108 and note <= 112) then
    if(note == 108) then
      if( velocity == 127) then
        autoDeckSelect = true
      else
        autoDeckSelect = false
      end
    else
      if(velocity==127) then
        deckSelected= note-108;
      end
    end

    deckSelectedNote = deckSelected+108

    for i=1,4 do
      if(deckSelected == i) then
        specialKeyBrightness[i+7] = padMaxBrightness
      else
        if(autoDeckSelect == true) then
          specialKeyBrightness[i+7] = 10
        else
          specialKeyBrightness[i+7] = 0
        end
      end
    end

    sendSpecialKeyBrightness()

    return
  end
end

function parseMidiMessageRight(x)
  local tag = x[1]&TAG_MASK
  local channel = (x[1] & CHANNEL_MASK)+1

  if (tag == TAG_NOTE_ON) then
    noteOnRight(channel, x[2], x[3])
  end
  if (tag == TAG_CONTROLLER) then
    controllerRight(channel, x[2], x[3])
  end
end


-------------------------------------------------------------------------------
-- Sending/Generating messages
-------------------------------------------------------------------------------

TAG_MASK = 0xF0
STATUS_MASK = 0x80
CHANNEL_MASK = 0x0F

TAG_NOTE_OFF         = 0x80
TAG_NOTE_ON          = 0x90
TAG_KEY_PRESSURE     = 0xA0
TAG_CONTROLLER       = 0xB0
TAG_PROGRAM          = 0xC0
TAG_CHANNEL_PRESSURE = 0xD0
TAG_PITCH_BEND       = 0xE0
TAG_SPECIAL          = 0xF0

MSG_SYSEX_START = 0xF0
MSG_SYSEX_END   = 0xF7

function generateMidiFirstByte(tag, channel)
  return tag|(channel-1)
end

function colorFlow()
  local padColorStateForPage = padColorState[page.currentPage]
  local padColorsForPage = padColors[page.currentPage]
  local padBrightnessStateForPage = padBrightnessState[page.currentPage]

  for pad=1,16 do
    local colorIndex = padColorStateForPage[pad][1]
    local colorStep = padColorStateForPage[pad][2]

    if(#padColorsForPage[pad] == 0) then
      local color = {0,0,0}

      if(padLastColor[pad]~=color) then
        sendHSB(sendMessageLeft, pad+padStartControllerNumberOffset, color[1], color[2], color[3])
        padLastColor[pad]=color
      end
    else


      if(colorStep==1) then
        local colorIndexInc = incrementRollOver(colorIndex,1,#padColorsForPage[pad])

        local rgbTo = {
          hsv2rgb(
            padColorsForPage[pad][colorIndexInc][1]/127,
            padColorsForPage[pad][colorIndexInc][2]/127,
            padBrightnessStateForPage[pad]/127
          )
        }

        local rgbFrom = {
          hsv2rgb(
            padColorsForPage[pad][colorIndex][1]/127,
            padColorsForPage[pad][colorIndex][2]/127,
            padBrightnessStateForPage[pad]/127
          )
        }

        for i=1,3 do
          padColorStateForPage[pad][3][i]=(rgbFrom[i]-rgbTo[i])/padColorsFlowSteps
        end

        padColorStateForPage[pad][4] = rgbFrom

        padColorStateForPage[pad][1] = colorIndexInc

        padColorStateForPage[pad][2] = 2

      else
        for i=1,3 do
          padColorStateForPage[pad][4][i]=padColorStateForPage[pad][4][i]-padColorStateForPage[pad][3][i]
        end
        padColorStateForPage[pad][2] = incrementRollOver(colorStep,1,padColorsFlowSteps)
      end

      local color = {rgb2hsv(
          padColorStateForPage[pad][4][1],
          padColorStateForPage[pad][4][2],
          padColorStateForPage[pad][4][3]
        )}

      for i=1,3 do
        color[i] = color[i]*127
      end


      if(padLastColor[pad]~=color) then
        sendHSB(sendMessageLeft, pad+padStartControllerNumberOffset, color[1], color[2], color[3])
        padLastColor[pad]=color
      end

    end
  end
end

function setNewBrightnessOnPad(brightness, brightnessNew)
  for j=1,3 do
    for i=1,16 do
      if(padBrightnessState[j][i]==brightness) then
        padBrightnessState[j][i]=brightnessNew
      end
    end
  end
end

function setMinBrightness(brightness)
  if(brightness~=padMaxBrightness) then
    setNewBrightnessOnPad(padMinBrightness,brightness)
    padMinBrightness=brightness
  end
end

function setMaxBrightness(brightness)
  if(brightness~=padMinBrightness) then
    setNewBrightnessOnPad(padMaxBrightness,brightness)
    padMaxBrightness=brightness
  end
end


--function sendPadColors()
--  local padColorsForPage = padColors[page.currentPage]
--  padColorIndex = incrementRollOver(padColorIndex, 1, 3)
--  for k=1,#padColorsForPage do
--    local color = padColorsForPage[k][padColorIndex]
--    if(padLastColor[k]~=color) then
--      sendHSB(sendMessageLeft, k+padStartControllerNumberOffset, color[1], color[2], nil)
--      padLastColor[k]=color
--    end
--  end
--end

--function sendPadBrightness()
--  local padBrightnessStateForPage = padBrightnessState[page.currentPage]

--  for k=1,#padBrightnessStateForPage do
--    local brightness = padBrightnessStateForPage[k]
--    local lastBrightness = padLastBrightness[k]
--    if(brightness~=lastBrightness) then
--      padLastBrightness[k] = brightness
--      sendHSB(sendMessageLeft, k+padStartControllerNumberOffset, nil, nil, brightness)
--    end
--  end
--end

function sendSpecialKeyBrightness()
  for k=1,#specialKeyBrightness do
    local brightness = specialKeyBrightness[k]
    local lastBrightness = specialKeyLastBrightness[k]
    if(brightness~=lastBrightness) then
      specialKeyLastBrightness[k] = brightness
      local controllerNum
      if(k<7) then
        controllerNum = k+79
      else
        controllerNum = k+29
      end
      sendController(sendMessageLeft, controllerNum, brightness)
    end
  end
end

function sendNoteOn(fun, note, velocity)
  fun({generateMidiFirstByte(TAG_NOTE_ON,13),note, velocity})
end

function sendController(fun, controller, value)
  fun({generateMidiFirstByte(TAG_CONTROLLER,13), controller, value})
end

function sendControllerForChannel(fun, channel, controller, value)
  fun({generateMidiFirstByte(TAG_CONTROLLER,channel), controller, value})
end

function sendHSB(fun, controller, hue, saturation, brightness)
  if(hue) then fun({generateMidiFirstByte(TAG_CONTROLLER,1),controller,hue}) end
  if(saturation) then fun({generateMidiFirstByte(TAG_CONTROLLER,2),controller,saturation}) end
  if(brightness) then fun({generateMidiFirstByte(TAG_CONTROLLER,3),controller,brightness}) end
end

function displayNumber(fun,number)
  fun({generateMidiFirstByte(TAG_CONTROLLER,13),42, number})
end

-------------------------------------------------------------------------------
-- general stuff
-------------------------------------------------------------------------------

function incrementRollOver(value, min, max)
  value = value+1;
  if(value>max) then value=min end
  if(value<min) then value=max end
  return value
end

function decrementRollOver(value, min, max)
  value = value-1;
  if(value>max) then value=min end
  if(value<min) then value=max end
  return value
end

-------------------------------------------------------------------------------
-- Data entry / system stuff
-------------------------------------------------------------------------------

function onMessageLeft (x)
  parseMidiMessage(x)
end

function onMessageRight (x)
  parseMidiMessageRight(x)
end

onFirstStart = true
function onTimer()
  if( onFirstStart == true ) then
    displayNumber(sendMessageLeft, 1)
    sendSpecialKeyBrightness()
    onFirstStart = false;
  end

  colorFlow()
end


-------------------------------------------------------------------------------
-- Color conversions
-------------------------------------------------------------------------------
function rgb2hsv( r, g, b )
  local M, m = math.max( r, g, b ), math.min( r, g, b )
  local C = M - m
  local K = 1.0/(6.0 * C)
  local h = 0.0
  if C ~= 0.0 then
    if M == r then     h = ((g - b) * K) % 1.0
    elseif M == g then h = (b - r) * K + 1.0/3.0
    else               h = (r - g) * K + 2.0/3.0
    end
  end
  return h, M == 0.0 and 0.0 or C / M, M
end

function hsv2rgb( h, s, v )
  local C = v * s
  local m = v - C
  local r, g, b = m, m, m
  if h == h then
    local h_ = (h % 1.0) * 6
    local X = C * (1 - math.abs(h_ % 2 - 1))
    C, X = C + m, X + m
    if     h_ < 1 then r, g, b = C, X, m
    elseif h_ < 2 then r, g, b = X, C, m
    elseif h_ < 3 then r, g, b = m, C, X
    elseif h_ < 4 then r, g, b = m, X, C
    elseif h_ < 5 then r, g, b = X, m, C
    else               r, g, b = C, m, X
    end
  end
  return r, g, b
end

-------------------------------------------------------------------------------
-- Initialisation
-------------------------------------------------------------------------------
for j=1,3 do
  for i=1,16 do
    padBrightnessState[j][i]=padMinBrightness
    padLastBrightness[i]=padMinBrightness+1
    padColorState[j][i] = {1,1,{0,0,0},{0,0,0}}
  end
end

for i=1,11 do
  specialKeyBrightness[i]=padMinBrightness
end

padColorsFlowSteps = 20
setTimeout(25)
displayNumber(sendMessageLeft, 1)
colorFlow()
sendSpecialKeyBrightness()
