
var j;
var i;
var fs = require ('fs');
var sr = [0x85,0xC0,0x74,0x08,0x33,0xDB,0x43];
var fn = "sublime_text.exe";

// print process.argv
process.argv.forEach(function(val, index, array) {
  console.log(index + ': ' + val);
});

console.log(process.env);

//对文件进行操作
(function(fn){
	fs.readFile(fn,function(err,data){
		if (err) throw err;
		console.log("File size:",data.length);
		console.log("Search len:",sr.length);
		for (i = 0;i< data.length; i++) {
			if (data[i]==sr[0]){
				for (j = 1;j<sr.length; j++) {
					if (data[j+i]!=sr[j]) {
						j=-1;
						break;
					};
				};
				
				if (j==sr.length){
					console.log("Found:",j+i-1,data[j+i-1]);
					data[j+i-1] = 0x90;
					fs.writeFile("x"+fn,data,function(err){
						if(err) throw err;
						console.log('Crack successfully!');
					})
				}
			}
		};
	});
})(fn);


