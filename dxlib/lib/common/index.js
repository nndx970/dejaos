import FFI from 'tjs:ffi';

let sopath = '/os/driver/';
sopath = sopath + './libcommon_wrapper.so';
const commonLib = new FFI.Lib(sopath);

const getUuid1 = new FFI.CFunction(commonLib.symbol('get_uuid'), FFI.types.string, [FFI.types.sint]);
const md5HashFile1 = new FFI.CFunction(commonLib.symbol('md5_hash_file'), FFI.types.string, [FFI.types.string]);

function getUuid(len) {
    return getUuid1.call(len);
}

function md5HashFile(file) {
    return md5HashFile1.call(file);
}

export { getUuid, md5HashFile };