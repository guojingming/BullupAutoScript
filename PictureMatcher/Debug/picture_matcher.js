var cdr = require("child_process");

cdr.exec("PictureMatcher.exe [TeamViewer] [D:/teamviewer.png]", (err, stdout, stderr) => {
   if (err) {
       console.error(err);
       return;
   }
   if (stderr){
       console.error(stderr);
       return;
   }
   var matchResult = JSON.parse(stdout.toString());
   console.log(matchResult);
   var a = 190;
});