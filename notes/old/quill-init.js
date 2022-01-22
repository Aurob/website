$(document).ready(function () {
    //Check if the url contains a document path
    //TODO
    //Any path can be entered, even if the document doesn't exist
    //disallow this and add a custom path input
    console.log(window.location.search)
    if (window.location.search) {
        sessionStorage["load"] = true;
        sessionStorage["filename"] = window.location.search.slice(1);
        ajax("load", sessionStorage["filename"]);

        $("#save").html("Update");
        $("#fileurl").show();
        filepath = window.location.origin + window.location.pathname + "?" + sessionStorage["filename"];
        $("#fileurl").html("<a href=" + filepath + ">" + filepath + "</a>");
    }
    else {
        sessionStorage["load"] = false;
        sessionStorage["filename"] = Math.random().toString(36).slice(2);
    }

    //This keeps the toolbar at the top of the screen
    var toolbar = document.getElementsByClassName("ql-toolbar")[0];
    var content = document.getElementsByClassName("content")[0];
    window.addEventListener('scroll', (e) => {
        if ($(window).scrollTop() + 100 >= $('.content').offset().top + $('.content').outerHeight() - window.innerHeight) {
            let doc_height = $(".content").height();
            $(".content").height(doc_height + 100);
        }
        if (window.pageYOffset > 33) {
            toolbar.classList.add("sticky");
            content.style.paddingTop = "66px";
        }
        else {
            toolbar.classList.remove("sticky");
            content.style.paddingTop = "0px";
        }
    });

    $("#save").click(function () {
        filename = sessionStorage["filename"];
        ajax("new", filename, JSON.stringify(quill.getContents().ops));

        $("#savenotif").show();
        $("#fileurl").show();
        filepath = window.location.origin + window.location.pathname + "?" + sessionStorage["filename"];
        $("#fileurl").html("<a href=" + filepath + ">" + filepath + "</a>");
        setTimeout(() => {
            $("#savenotif").hide();
        }, 1e4);
    });

    $("#new").click(function () {
        window.open('https://robauis.me/editor/', '_blank');
    });

});

function ajax(func, filename, content = null) {
    let xml = new XMLHttpRequest();
    xml.onreadystatechange = () => {
        if (xml.readyState == XMLHttpRequest.DONE) {
            let data = xml.response;
            if (func == "load") {
                let content = JSON.parse(JSON.parse(data).content);
                quill.setContents(content);
            }
            else {
                console.log(JSON.parse(data));
            }
        }
    }
    xml.open("POST", "index.php", true);
    xml.setRequestHeader('Content-type', 'application/x-www-form-urlencoded'); //URL encoded form
    xml.send("function=" + func + "&filename=" + filename + "&content=" + content);
}