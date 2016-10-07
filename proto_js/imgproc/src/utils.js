function copyPixels(src, dst, w, h, r) {
	for(var i = 0; i < w * h * r; i += r) {
		for(var j = 0; j < r; j ++) {
			dst[i + j] = src[i + j];
		}
	}
}

function clearPixels(dst, w, h, r) {
	for(var i = 0; i < w * h * r; i += r) {
		for(var j = 0; j < r; j ++) {
			dst[i + j] = 0;
		}
	}
}

function channel4To1(src, w, h) {
	var dst = [];
	for(var i = 0, j = 0; i < w * h * 4; i += 4, j ++) {
		dst[j] = src[i];
	}
	return dst;
}