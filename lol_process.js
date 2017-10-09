var child_process = require("child_process");

exports.grabLOLData = function(type, socket){
    child_process.exec('node C:/Users/Public/Bullup/auto_program/sync_lol_' + type + '_process.js', function (error, stdout, stderr) {
        var fs = require('fs');
        if (error) {
            // console.log(error.stack);
            // console.log('Error code: '+error.code);
            // console.log('Signal received: '+error.signal);
            fs.writeFileSync("C:/Users/Public/Bullup/auto_program/BullupError.txt", error);
        }
        if(stderr){
            //console.log('sync_lol_process stderr: ' + stderr);
            fs.writeFileSync("C:/Users/Public/Bullup/auto_program/BullupStdError.txt", stderr);
        }
        stdout = JSON.parse(stdout);
        //fs.writeFileSync("C:/Users/Public/Bullup/auto_program/BullupStdout.txt", JSON.parse(stdout));
        var packet;
        if(stdout.UserInfo != undefined){
            packet = processLoginPacket(stdout);
            socket.emit('lolLoginResult', packet);
        //}else if(stdout.BattleInfo != undefined){
        }else if(stdout.actions != undefined){
            packet = processRoomPacket(stdout);
            socket.emit('lolRoomEstablished', packet);
        }else if(stdout.gameMode != undefined){
            packet = processResultPacket(stdout);
            socket.emit('lolBattleResult', packet);
        }
        //console.log(packet); 
    });
}


function processLoginPacket(stdout){
    var loginPacket = {};
    var rankTierInfo = String(stdout.UserInfo.rankedTierInfo);
    var ranks = ['UNRANKED','BRONZE','SILVER','GOLD','PLATINUM','DIAMOND','MASTER','CHALLENGER'];
    loginPacket.currentRank = 'UNRANKED';
    for(var index in ranks){
        if(rankTierInfo.indexOf(ranks[index]) != -1){
            loginPacket.currentRank = ranks[index];
            break;
        }
    }
    loginPacket.head = "user";
    loginPacket.accountId = stdout.UserInfo.userId;
	loginPacket.nickname = stdout.UserInfo.displayName;
    loginPacket.lastRank = stdout.UserInfo.lastSeasonRank;
    loginPacket.serverName = stdout.UserInfo.serverName;
    //{head: "user", accountId: 2936285067, nickname: "Spa丶", lastRank: "UNRANKED", currenRank: "SILVER", serverName: "外服"}
    return loginPacket;
}

function processRoomPacket(stdout){
    var roomPacket = {};
    roomPacket.head = "room";
    // stdout = stdout.BattleInfo.gameData;
    // roomPacket.myTeam = stdout.teamOne;
    // roomPacket.theirTeam = stdout.teamTwo;
    roomPacket.myTeam = stdout.myTeam;
    roomPacket.theirTeam = stdout.theirTeam;
    return roomPacket;
}

function processResultPacket(stdout){
    var resultPacket = {};
    resultPacket.head = "result";
    resultPacket.accountId = stdout.accountId;
    resultPacket.gameMode = stdout.gameMode;
    resultPacket.gameType = stdout.gameType;
    if(stdout.teams[0].players[0].stats.WIN == 1){
        resultPacket.win = "yes";
    }else{
		resultPacket.win = "no";
	}
    return resultPacket;
}

exports.grabLOLData("login", null);
//exports.grabLOLData("room", null);