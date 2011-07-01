/*
 *  Copyright 2011 Samsung Electronics, Incorporated.
 *  Advanced Browser Technology Project
 */
 
function InitJS(canvasName) {
    var canvas = document.getElementById(canvasName);
    var ctx = canvas.getContext("2d");
    
    if(ctx === null) {
        console.error("Failed to create Canvas2D context");
        return null;
    }
    
    // needed
    canvas.width  = WINW;
    canvas.height = WINH;
    
    return ctx;
}
 
function DrawJS(ctx) {
    if(ctx === null)
        return;
        
    ctx.fillStyle = 'rgba(0,0,0,1)';
    ctx.fillRect(0, 0, WINW, WINH);
        
    ctx.fillStyle = 'rgba(255,255,0,1)';

    for (var i=0; i < NBODY; i++)  {
        var x = userData.curPos[4*i+0];
        var y = userData.curPos[4*i+1];
        var z = userData.curPos[4*i+2];
        
        // use GL orientation
        y = -y;

        var px = (WINW + (x * WINW))/2;
        var py = (WINH + (y * WINH))/2;
        var pz = (WINH + (z * WINH))/2;
        var pr = 4 * (pz/WINH);
        
        if(pr < 0) pr = 1;
        if(pr > 4) pr = 4;
        
        ctx.beginPath();
        ctx.arc(px, py, pr, 0, Math.PI*2, true);
        ctx.closePath();
        ctx.fill(); 
    }
}