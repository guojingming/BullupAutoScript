var robot = require("robotjs");

exports.moveMouseToLocationAndDClick = function(x, y){
    robot.moveMouse(x, y);
    robot.mouseClick("left", true);
}

exports.inputText = function(text){
	robot.typeStringDelayed(text, 200);
}

exports.keyPressed = function(key, count){
    var tempCount = 0;
    while(tempCount++ < count){
        robot.keyTap(key);
    }
}