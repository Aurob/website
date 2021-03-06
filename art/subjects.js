/**
 * subjects.js 
 * Version: 3
 * Builds a basic configurable webpage from a YAML config file
 */

var subjects = {}
var subject_icons = {};
var subject_scripts = {};
var default_subject;
var default_logo;
var media_path;
var pageid;

$(document).ready(function () {
    $("#resume iframe").css("height", "500px");
    $("iframe").css("height", window.innerHeight + "px");
    if (window.location.hash) {
        pageid = window.location.hash.slice(1);
    }
    else pageid = "#";
    init(pageid);
});

function init(pageid) {

    // fetch("subjects.json")
    //     //read the config file as json
    //     .then(response => response.json())
    //populate the subjects array
    fetch("subjects.yml", {
        method: 'GET', // or 'PUT'
        headers: {
            'Content-Type': 'text/yaml',
        }
    }).then(res => res.text())
        .then(yaml => jsyaml.load(yaml))
        .then(data => load_subjects(data))
        //load the current subject
        .then(load => load_page(pageid))
        //add subject link click events
        .then(events => {
            $("body").on("click", ".link", function (e) {
                switch (e.which) {
                    case 1:
                        load_page(e.target.hash.slice(1));
                    default:
                        break;
                }
            });
        })
}
function load_page(id) {
    //redirect blank paths to home
    if (!(id in subjects)) {
        id = default_subject;
        window.location.hash = id;
    }

    if ("nav_logo" in subjects[id]) {
        if (subjects[id].nav_logo.endsWith(".js")) {
            $("#logo").parent().append("<script src='" + subjects[id].nav_logo + "'><\/script>");
        }
        else {
            $("#logo")[0].src = media_path + subjects[id].nav_logo;
        }
    }
    else $("#logo")[0].src = media_path + default_logo;

    $("#subject_block").html(subjects[id].block);
    if (id in subject_scripts) {
        Object.keys(subject_scripts[id]).forEach(key => {
            
            let script = subject_scripts[id][key];
            
            if (script.endsWith(".js"))
                $("#subject_block").append("<script src='" + script + "'><\/script>");
            else if (script.startsWith("<script>") && script.endsWith("<\/script>")) {
                $("#subject_block").append(script);
            }
            else {
                $("#subject_block").append("<script>" + script + "<\/script>");
            }
        });
        
    }

    $("#subject_block").css("width", subjects[id].width)
    $(".page").css("display", "none");
    $("#" + id).css("display", "flex");

}

function load_subjects(data) {
    if ("header" in data) {
        if ("title" in data.header) {
            document.title = data.header.title;
        }
        if ("nav_logo" in data.header) {
            default_logo = data.header.nav_logo;
        }
        if ("media_path" in data.header) {
            media_path = data.header.media_path;
        }
        
        $("body").css("width", data.header.width)
    }

    for (subject in data.data) {
        subject = data.data[subject];
        Object.keys(subject).forEach(key => {
            if (key.indexOf("script") > -1) {
                if(!subject_scripts[subject.id]) subject_scripts[subject.id] = [];
                subject_scripts[subject.id].push(subject[key]);
            }
        })
        if ("default" in subject && subject.default) {
            default_subject = subject.id;
        }
        //Add subject to nav list
        $("#nav-list").append(`<div class="flex-item">
                <a class="link" href="`+ ((subject.id == "home") ? '/#' + subject.id : '#' + subject.id) + `">` + subject.subject + `</a>
            </div>`);

        //Contstruct subject body and add to subjects array
        let subject_header =
            '<div id="header">' +
            '<h1>' + subject.title + '</h1>' +
            '<small>' + subject.sub_title + '</small>' +
            '<hr>'
        '</div>';

        var subject_content = [];
        for (i = 0; i < Object.keys(subject).length; i++) {
            let key = Object.keys(subject)[i];
            if (key.startsWith('content')) {
                if (subject[key].match(/\w+\.html/)) {
                    subject_content.push("<iframe src='" + subject[key] + "'></iframe>")
                }
                else {
                    subject_content.push(subject[key]);
                }

            }
        }
        subject_content = subject_content.join('');
        let subject_image = '';

        if (subject.image) {
            if (typeof (subject.image) == "string") {
                subject_image = '<img loading=\'lazy\' src="' + media_path + subject.image + '"</img>';
            }
            else {
                for (image in subject.image) {
                    subject_image += '<img loading=\'lazy\' src="' + media_path + subject.image[image] + '"</img>';
                }
            }
        }

        let subject_block =
            '<div id="' + subject.id + '" class="page flex-item">' +
            subject_header +
            subject_content +
            ((subject.image != '') ? subject_image : '') +
            '</div>';

        // subjects[subject.id] = subject_block;
        subject.block = subject_block;
        subjects[subject.id] = subject;
    }
}
