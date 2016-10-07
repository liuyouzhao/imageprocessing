var gravect = function() {}

gravect.__direct_vector_num = 9;
gravect.__direct_vector_range = 180 / 9;
gravect.__norm_e = 0.01;

gravect.extract = function(gw, gd, w, h, wnd, b, c) {
	return __gravect_extract(gw, gd, w, h, wnd, b, c);
}
/*
* Main processing function
*/
function __gravect_extract(gw, gd, w, h, wnd, b, c) {
	var gw1 = channel4To1(gw, w, h);
	var gd1 = channel4To1(gd, w, h);

	var nw = w / wnd;
	var nh = h / wnd;

	var v = [];
	var res = [];
	var n = 0;
	for(var i = 0; i < nh; i ++) {
		for(var j = 0; j < nw; j ++) {
			var imgww = getPart(gw1, j * wnd, i * wnd, wnd, w, h);
			var imgwd = getPart(gd1, j * wnd, i * wnd, wnd, w, h);
			if(isEmpty(imgww, wnd, wnd))  {
				continue;
			}
			v = getWindowVector(imgww, imgwd, wnd, b, c);
			res[n] = v;
			n ++;
		}
	}

	/*=================================================================================*/
	return res;
}

function getCellVector(imgw, imgd, w) {
	var v = [];
	for(var i = 0; i < gravect.__direct_vector_num; i ++) {
		v[i] = 0;
	}

	for(var i = 0; i < w * w; i ++) {
		var wt = imgw[i];
		var dir = imgd[i];
		var d = Math.round(dir / gravect.__direct_vector_range);
		v[d] = v[d] + wt;
	}

	return v;
}

function getBlockVector(imgw, imgd, bw, cw) {
	var nw = bw / cw;
	var v = [];
	var n = 0;
	for(var i = 0; i < nw; i ++) {
		for(var j = 0; j < nw; j ++) {
			var cellImgW = getPart(imgw, j * cw, i * cw, cw, bw, bw);
			var cellImgD = getPart(imgd, j * cw, i * cw, cw, bw, bw);

			var vc = getCellVector(cellImgW, cellImgD, cw);
			for(var i2 = 0; i2 < gravect.__direct_vector_num; i2 ++) {
				v[n] = vc[i2];
				n ++;
			}
		}
	}
	v = l2Norm(v, n);
	return v;
}

function getWindowVector(imgw, imgd, ww, bw, cw) {
	var nw = ww / bw;
	var v = [];
	var n = 0;
	var nc = bw / cw;
	var nc2 = nc * nc;
	for(var i = 0; i < nw; i ++) {
		for(var j = 0; j < nw; j ++) {
			var blockImgW = getPart(imgw, j, i, bw, ww, ww);
			var blockImgD = getPart(imgd, j, i, bw, ww, ww);

			var vb = getBlockVector(blockImgW, blockImgD, bw, cw);
			for(var i2 = 0; i2 < gravect.__direct_vector_num * nc2; i2 ++) {
				v[n] = vb[i2];
				n ++;
			}
		}
	}
	return v;
}

function l2Norm(v, size) {
	var n = 0;
	for(var i = 0; i < size; i ++) {
		n = n + v[i] * v[i];
	}
	n = Math.sqrt(n);
	var n2 = n*n + gravect.__norm_e*gravect.__norm_e;
	var res = Math.sqrt(n2);

	for(var i = 0; i < size; i ++) {
		v[i] = v[i] / res;
	}

	return v;
}

function getPart(img, sx, sy, wnd, w, h) {
	var res = [];
	var x = 0;
	for(var i = sy; i < sy + wnd; i ++) {
		for(var j = sx; j < sx + wnd; j ++) {
			res[x] = img[i * w + j];
			x ++;
		}
	}
	return res;
}

function isEmpty(p, w, h) {
	var num = 0;
	for(var i = 0; i < h * w; i ++) {
		if(p[i] > 0) {
			num ++;
		}
		if(num > w * h / 10) {
			return false;
		}
	}
	return true;
}