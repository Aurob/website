var table_template;
fetch("i1.html")
    .then(response => response.text())
    .then(html => {
        table_template = html;
});
function getServerList(servers) {
    
}

var ws = null;
if(sessionStorage.getItem("uid") == null) {
    sessionStorage.setItem("uid", String(Math.floor(Math.random() * 100000)));
}

url = "wss://robauis.me/ws/";
ws = new WebSocket(url);

var onMessage = function(event) {
    var ws_data = JSON.parse(event.data);
    console.log( ws_data );
    if(ws_data.type == "serverlist") {
        let servers = ws_data.servers;
        $('#data').html("");
        table = $(table_template)
        let header_tr = $("<tr><th><th>ID</th></<th><th>Server</th><th>Members</th><th>Map</th></tr>");
        table.find("tbody").append(header_tr);
        servers.forEach(server => {
            let member_tr = $("<tr><td class='active-srv'></td></tr>");
            Object.keys(server).forEach(key => {
                let key_row = $("<td></td>");
                if(key == "name") {
                    //server[key]
                    key_btn = $("<button class='joinbtn'>Join "+server["name"] +"</button>");
                    key_row.append(key_btn);
                    if(server["id"] == sessionStorage.getItem("session_server")) {
                        if(server["map"]!="") {
                            key_room = $("<button class='joinbtn'>Enter "+server["map"]+"</button>");
                            key_row.append(key_room);

                            key_room.on("click", function() {
                                sessionStorage.setItem("session_room", server["map"]);
                                window.open("/test/build/"+server["map"]+"/"+server["map"]+".html");
                            });
                        }
                        
                        key_btn.html("Leave "+server["name"]);
                    }
                    key_btn.on("click", function() {
                        if(server["id"] == sessionStorage.getItem("session_server")) {
                            member_tr.find(".active-srv").html("*");
                            if(confirm("Leave " + server[key] + "?")) {
                                ws.send(JSON.stringify({'leaveserver': server["name"], 'member': sessionStorage.getItem("uid")}));
                                sessionStorage.removeItem("session_room");
                                sessionStorage.removeItem("session_server");
                                table.find(".active-srv").html("");
                            }
                        }
                        else if(confirm("Join " + server[key] + "?")) {
                            sessionStorage.setItem("session_server", server["id"]);
                            ws.send(JSON.stringify({'joinserver': server["name"], 'member': sessionStorage.getItem("uid")}));
                            key_btn.html("Join "+server["name"]);
                            table.find(".active-srv").html("");
                            member_tr.find(".active-srv").html("*");
                        }    
                    });

                    if(server["id"] == sessionStorage.getItem("session_server")) 
                        member_tr.find(".active-srv").html("*");
                }
                else {
                    key_row.html(server[key]);
                }

                member_tr.append(key_row);
            });
            table.find("tbody").append(member_tr);
        });

        $('#data').html(table);
    }
    ws.send({"update":false})
};
var onOpen = function(event) {
    connected = true;
    $("#srvbtn").on("click", function(){
        $(this).html("Refresh Servers");
        ws.send(JSON.stringify({'listservers':true}));
        // loadServerList();
    });
};
var onClose = function(event){
    console.log("Ending WS Connection...");
    // loadServerList();
};
ws.onerror = function(e) {
    console.log("self.socket event.type", e);
  };
ws.onclose = onClose;
ws.onopen = onOpen;
ws.onmessage = onMessage;