"use strict";

var canvas; // The canvas on which WebGL draws.
var holder; // a div to hold the canvas element.
var gl;     // The WebGL graphics context.

var prog; // The shader program.

var POINT_COUNT = 200;
var POINTSIZE_MIN = 50;
var POINTSIZE_MAX = 255;
var ALPHA_MIN = 0.2;
var ALPHA_MAX = 0.7;
var SPEED_MIN = 0.005;
var SPEED_MAX = 0.015;

var aPointSize;    // Location of the "pointSize" attribute in the shader program.
var aColor;        // Location of the "color" attribute in the shader program.
var aCoords;       // Location of the "coords" attribute in the shader program.
var aColorBuffer;  // Data buffer to hold values for "color" attribute.
var aCoordsBuffer; // Data buffer to hold values for "coords" attribute.
var aPointSizeBuffer;  // Data buffer to hold values for "coords" attribute.

var coords = new Float32Array(2*POINT_COUNT);
var velocities = new Float32Array(2*POINT_COUNT);
var colors = new Float32Array(4*POINT_COUNT);
var pointSizes = new Uint8Array(POINT_COUNT);

var fragmentShaderSource =  // Source code for the fragment shader.
    "precision mediump float;\n" + 
    "varying vec4 vColor;\n" +
    "void main() {\n" +
    "    float dist = distance( vec2(0.5,0.5), gl_PointCoord );\n" +
    "    if ( dist > 0.5 )\n" +
    "       discard;\n" +
    "    gl_FragColor = vColor;\n" +
    "}\n";

var vertexShaderSource =  // Source code for the vertex shader.
    "attribute vec2 coords;\n" +
    "attribute vec4 color;\n" +
    "attribute float pointSize;\n" +
    "varying vec4 vColor;\n" +
    "void main() {\n" +
    "    vColor = color;\n" +
    "    gl_Position = vec4( coords, 0.0, 1.0 );\n" +
    "    gl_PointSize = pointSize;\n" +
    "}\n";

    
/* Render the image. This is called in init() and also when the
 * user changes the selection in either of the two popup menus.
 * In this program, it just draws a regular polygon.  The number of
 * sides and the color of the polygon are selected by the user
 * using popup menus (i.e. select elements).
 */
function draw() {
   
    gl.clearColor(1,1,1,1);
    gl.clear(gl.COLOR_BUFFER_BIT);
    
    gl.enableVertexAttribArray(aCoords);
    gl.bindBuffer(gl.ARRAY_BUFFER, aCoordsBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, coords, gl.STREAM_DRAW);
    gl.vertexAttribPointer(aCoords, 2, gl.FLOAT, false, 0, 0);
    
    gl.drawArrays(gl.POINTS, 0, POINT_COUNT);

} // end draw()

function updateForNextFrame() {
    for (var i = 0; i < coords.length; i++) {
        coords[i] += velocities[i];
        if (coords[i] < -1) {
           coords[i] = -1;
           velocities[i] = Math.abs(velocities[i]);
        }
        else if (coords[i] > 1) {
           coords[i] = 1;
           velocities[i] = -Math.abs(velocities[i]);
        }
    }
}

function setupShaderVars() {

    /* Get the locations for shader program variables */
    aCoords = gl.getAttribLocation(prog, "coords");
    aColor = gl.getAttribLocation(prog, "color");
    aPointSize = gl.getAttribLocation(prog, "pointSize");
    
    /* Create buffers to hold data for the attribute variables. */
    aColorBuffer = gl.createBuffer();
    aCoordsBuffer = gl.createBuffer();
    aPointSizeBuffer = gl.createBuffer();
    
    /* Set up data arrays. */
    for (var i = 0; i < POINT_COUNT; i++) {
        pointSizes[i] = Math.round(POINTSIZE_MIN + (POINTSIZE_MAX - POINTSIZE_MIN)*Math.random());
        coords[2*i] = -1 + 2*Math.random();
        coords[2*i+1] = -1 + 2*Math.random();
        var speed = SPEED_MIN + (SPEED_MAX - SPEED_MIN)*Math.random();
        var angle = 2*Math.PI*Math.random();
        velocities[2*i] = Math.cos(angle) * speed;
        velocities[2*i+1] = Math.sin(angle) * speed;
        colors[4*i] = Math.random();
        colors[4*i+1] = Math.random();
        colors[4*i+2] = Math.random();
        colors[4*i+3] = ALPHA_MIN + (ALPHA_MAX - ALPHA_MIN)*Math.random();
    }
    
    gl.enableVertexAttribArray(aColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, aColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, colors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(aColor, 4, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(aPointSize);
    gl.bindBuffer(gl.ARRAY_BUFFER, aPointSizeBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, pointSizes, gl.STATIC_DRAW);
    gl.vertexAttribPointer(aPointSize, 1, gl.UNSIGNED_BYTE, false, 0, 0);
    
}


/* Creates a program for use in the WebGL context gl, and returns the
 * identifier for that program.  If an error occurs while compiling or
 * linking the program, an exception of type String is thrown.  The error
 * string contains the compilation or linking error.  If no error occurs,
 * the program identifier is the return value of the function.
 */
function createProgram(gl, vertexShaderSource, fragmentShaderSource) {
   var vsh = gl.createShader( gl.VERTEX_SHADER );
   gl.shaderSource(vsh,vertexShaderSource);
   gl.compileShader(vsh);
   if ( ! gl.getShaderParameter(vsh, gl.COMPILE_STATUS) ) {
      throw "Error in vertex shader:  " + gl.getShaderInfoLog(vsh);
   }
   var fsh = gl.createShader( gl.FRAGMENT_SHADER );
   gl.shaderSource(fsh, fragmentShaderSource);
   gl.compileShader(fsh);
   if ( ! gl.getShaderParameter(fsh, gl.COMPILE_STATUS) ) {
      throw "Error in fragment shader:  " + gl.getShaderInfoLog(fsh);
   }
   var prog = gl.createProgram();
   gl.attachShader(prog,vsh);
   gl.attachShader(prog, fsh);
   gl.linkProgram(prog);
   if ( ! gl.getProgramParameter( prog, gl.LINK_STATUS) ) {
      throw "Link error in program:  " + gl.getProgramInfoLog(prog);
   }
   return prog;
}

//--------------------- Animation support -----------------------------------

/**
 * The following statement makes sure that requestAnimationFrame
 * is available as a fuction, using a browser-specific version if 
 * available or falling back to setTimeout if necessary.  Call
 * requestAnimationFrame(callbackFunction) to set up a call to
 * callbackFunction.  callbackFunction is called with a parameter
 * that gives the current time.  
 */
window.requestAnimationFrame = 
    window.requestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.msRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    function (callback) {
        setTimeout(function() { callback(Date.now()); },  1000/60);
    }
    
var animating = false;

function animate() {
    if (!animating)
       return;
    updateForNextFrame();
    draw();
    requestAnimationFrame(animate);
}
    
//-------------------------------------------------------------------------


/* Call this function to add an overalay of moving disks to the web page.
 */
function addpoints() {

    if (holder)
       return;

    if (!canvas) {
        try {
            /* Initialize the WebGL graphics context for drawing on the canvas. */
            canvas = document.createElement("canvas");
            gl = canvas.getContext("webgl", {alpha:false});
            if ( ! gl ) {
                gl = canvas.getContext("experimental-webgl", {alpha:false});
            }
        }
        catch (e) {
            gl = null;
        }
    
        if (!gl) {
            alert("Sorry, your computer/browser can't do WebGL.");
            return;
        }
        
        
        try {
            /* Create the shader program */
            prog = createProgram(gl, vertexShaderSource, fragmentShaderSource);
        }
        catch (e) {
            alert("Sorry, my WebGL program doesn't work on this machine.");
            return;
        }

        if ( POINTSIZE_MAX > gl.getParameter(gl.ALIASED_POINT_SIZE_RANGE)[1] ) {
                // if large points aren't availble, use the maximum point size
            POINTSIZE_MAX = gl.getParameter(gl.ALIASED_POINT_SIZE_RANGE)[1];
            POINTSIZE_MIN = Math.round(POINTSIZE_MAX * 0.25);
        }

        setupShaderVars();
    
        /* Tell the graphics context to use the shader program for drawing. */
        gl.useProgram(prog);
    
        gl.blendFuncSeparate( gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA,
                             gl.ZERO, gl.ONE );
        gl.enable(gl.BLEND);
    
        /* Call the draw routine to render the image. */
        canvas.style.position = "fixed";
        canvas.style.left = "0px";
        canvas.style.top = "0px";
        canvas.style.opacity = "0.3";
        canvas.style.zIndex = "100";
    }
    
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    gl.viewport(0, 0, window.innerWidth, window.innerHeight);
    window.onresize = function() {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        gl.viewport(0, 0, window.innerWidth, window.innerHeight);
        draw();
    }
    holder = document.createElement("div");
    holder.appendChild(canvas);
    document.body.appendChild(holder);
    animating = true;
    animate();

} // end addpoints()


function removePoints() {
    if (!holder)
       return;
    animating = false;
    document.body.removeChild(holder);
    window.onresize = null;
    holder = null;
}


