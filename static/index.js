const socket = io()
socket.emit("load")

// Port
socket.on("ports", function(data) {
    $("#ports").html("")
    data.forEach((port) => {
        $("#ports").append(
            `<option value="${port}">${port}</option>`
        )
    })
})


// 2D Input
function makeDraggable(evt) {
    var svg = evt.target;
    svg.addEventListener('mousedown', startDrag);
    svg.addEventListener('mousemove', drag);
    svg.addEventListener('mouseup', endDrag);
    // svg.addEventListener('mouseleave', endDrag);
        
    function getMousePosition(evt) {
        var CTM = svg.getScreenCTM();
        return {
            x: (evt.clientX - CTM.e) / CTM.a,
            y: (evt.clientY - CTM.f) / CTM.d
        };
    }

    var isDragging = false

    function startDrag(evt) {
        isDragging = true
        send(evt)
    }
    function drag(evt) {
        if (isDragging)
            send(evt)
    }
    function endDrag(evt) {
        isDragging = false
    }

    function send(evt) {
        
        var coord = getMousePosition(evt);
        var r = 50;
        var cx = 150;
        var cy = 150;
        var dist = Math.sqrt((coord.x - cx) ** 2 + (coord.y - cy) ** 2);
        if (dist > r) {
            var angle = Math.atan2(coord.y - cy, coord.x - cx);
            coord.x = cx + r * Math.cos(angle);
            coord.y = cy + r * Math.sin(angle);
        }
        var selectedElement = document.getElementById("target");
        selectedElement.setAttributeNS(null, "cx", coord.x-cx+r);
        selectedElement.setAttributeNS(null, "cy", coord.y-cy+r);

        var realx = (coord.x-cx)/r;
        var realy = (coord.y-cy)/r;
        socket.emit("xy", realx, realy);
        var output = document.getElementById("value");
        output.innerHTML = `(${Math.round(realx)}, ${Math.round(realy)}, 0)`;
    }
}
$().ready(function() {


$("#openBtn").click(function() {
    socket.emit('port', true)
    $(this).css('display', 'none')
    $("#closeBtn").css('display', 'block')
    $("#startBtn").css('display', 'block')
})

$("#closeBtn").click(function() {
    socket.emit('port', false)
    $(this).css('display', 'none')
    $("#openBtn").css('display', 'block')
    $("#startBtn").css('display', 'none')
})

// Slider
var sliders = document.getElementsByClassName("slider")
Array.from(sliders).forEach((slider) => {
    slider.oninput = function() {
        if (this.id == "rangeAll") {
            socket.emit("z", this.value);
        } else {
            const id = parseInt(this.id[this.id.length - 1])
            console.log(id, this.value)
            socket.emit("individual", id, this.value)
        }
    }
})
})