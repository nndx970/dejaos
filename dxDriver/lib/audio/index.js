import FFI from 'tjs:ffi';

let sopath = '/os/driver/';
sopath = sopath + './libaudio_wrapper.so';
const audioLib = new FFI.Lib(sopath);

const audioInit1 = new FFI.CFunction(audioLib.symbol('audio_init'), FFI.types.void, [FFI.types.uint32]);
const audioDeinit1 = new FFI.CFunction(audioLib.symbol('audio_deinit'), FFI.types.void, []);
const audioPlay1 = new FFI.CFunction(audioLib.symbol('audio_play'), FFI.types.void, [FFI.types.string]);
const audioPlayingInterrupt1 = new FFI.CFunction(audioLib.symbol('audio_playing_interrupt'), FFI.types.void, []);
const audioGetVolume1 = new FFI.CFunction(audioLib.symbol('audio_get_volume'), FFI.types.uint32, []);
const audioSetVolume1 = new FFI.CFunction(audioLib.symbol('audio_set_volume'), FFI.types.void, [FFI.types.uint32]);


const audioGetVolumeRange1 = new FFI.CFunction(audioLib.symbol('audio_get_volume_range'), FFI.types.uint32, [FFI.types.buffer, FFI.types.buffer]);


function audioInit(volume = 30) {
    return audioInit1.call(volume);
}
function audioDeinit() {
    return audioDeinit1.call();
}
function audioPlay(wav_path) {
    return audioPlay1.call(wav_path);
}
function audioPlayingInterrupt() {
    return audioPlayingInterrupt1.call();
}
function audioGetVolume() {
    return audioGetVolume1.call();
}
function audioSetVolume(volume) {
    return audioSetVolume1.call(volume);
}
function audioGetVolumeRange() {
    const max = new Buffer(4);
    const min = new Buffer(4);
    audioGetVolumeRange1.call(max, min);
    console.log('max:', FFI.bufferToNumber(max));
    console.log('min:', FFI.bufferToNumber(min));
}

export { audioInit, audioDeinit, audioPlay, audioPlayingInterrupt, audioGetVolume, audioSetVolume, audioGetVolumeRange };