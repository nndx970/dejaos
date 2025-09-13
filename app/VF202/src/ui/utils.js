let hideStyle = {
    display: "none",
}
let showStyle = {
    display: "block",
}

function hide(obj) {
    obj.nativeSetStyle(hideStyle, Object.keys(hideStyle), Object.keys(hideStyle).length, 0, true)
}

function show(obj) {
    obj.nativeSetStyle(showStyle, Object.keys(showStyle), Object.keys(showStyle).length, 0, true)
}

export { hide, show }