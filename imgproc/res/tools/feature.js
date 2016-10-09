var fs=require('fs');
var path = require('path');
var exec = require('child_process').exec; 

var src = null;

var cmd = "../../bin/Debug/imgproc";

process.argv.forEach(function (val, index, array) {
    if(index == 2) src = val;
});

console.log(src);

function ls(ff)
{
    var files=fs.readdirSync(ff);
    for(fn in files)
    {
        var fname = ff+path.sep+files[fn];
	var stat = fs.lstatSync(fname);
        if(stat.isDirectory() == true)
        {
            ls(fname);
        }
        else
        {
            //console.log(fname);
 	    pro(fname);
        }
    }
}

function pro(name) {

    //var free = spawn(cmd, [name]);
    var free = exec('ll');

    free.stdout.on('data', function (data) {
        console.log(data);
    });

    free.stderr.on('data', function (data) {
        console.log(data);
    });

    free.on('exit', function (code, signal) {
        console.log('Task finished: ' + code);
    }); 
}



ls(src);
