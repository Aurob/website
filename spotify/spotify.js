$(document).ready(function() {
    function get_current_song() {
        return fetch("https://api.spotify.com/v1/me/player/currently-playing", {
            "headers": {
                'Accept': 'application/json',
                'Content-Type': 'application/json',
                "Authorization": "Bearer BQC0G43JkU43Hmm-B2D00NUOqBvAP1iWr2EblGIyct_tEMlfZVzabmwIgLyv8Z5O_8HUIKdkmP_ykIeVRfbB-P4CzoZ4G-7c6acxO9reDKU9aDuTV6NaOiXVPRHOSUg2uvPz4eIXw6P7MaRFEvwRCJ3j2oIrLnSZWPCaIDBj",
            },
            "referrer": "https://robauis.me/spotify",
            "method": "GET",
            "mode": "cors"
        })
        .then(res => res.json())
        .then(data => {
            return data;
        });
    } 

    function set_current_song() {
        get_current_song().then(data => {
            let track_id = data.item.id;
            let embed = `<iframe 
                            src="https://open.spotify.com/embed/track/${track_id}" 
                            width="100%" height="80" frameBorder="0" allowfullscreen="">
                        </iframe>`
            $('#current-song').html(embed);

            let song_progress = data.progress_ms;
            let song_duration = data.item.duration_ms;
            let next_song_ms = song_duration - song_progress;
            if(next_song_ms != 0) {
                setTimeout(function(){set_current_song()}, next_song_ms);
            }
            // console.log(data);
        });
    }

    set_current_song();
});