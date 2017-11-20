var process = require("child_process");
var fs = require("fs");


process.execSync('C:/Users/Public/Bullup/auto_program/BullupServiceNew BattleInfo');
console.log(fs.readFileSync('C:/Users/Public/Bullup/log.txt').toString());