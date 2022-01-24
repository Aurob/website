<?php
header('Content-Type: application/json');

// ini_set('display_errors', 1);
// ini_set('display_startup_errors', 1);
// error_reporting(E_ALL);

$response = array();
$art_count = 0;

if(isset($_GET['count'])){   
    $art_count = $_GET['count']; 
}

$art_list = [];
if($art_count > 0) {
    
    $art_ids = [];
    $db = new SQLite3('/var/www/html/art/artvee.db');
    $count = $db->querySingle("SELECT COUNT(*) as count FROM art");

    for($i = 0; $i < $art_count; $i++) {
        $rand_row = rand(0, $count-1);
        if(!in_array($rand_row, $art_ids)) {

            $res = $db->query('SELECT * FROM art WHERE rowid = '.$rand_row);
            while ($row = $res->fetchArray()) {
                $url = $row['link'];
                $img = file_get_contents($url);
                $b64image = "data:image/jpeg;base64," . base64_encode($img);
                $art_data['image_b64'] = $b64image;
                $art_data['size'] = getimagesize($b64image);
            }
            array_push($art_ids, $rand_row);
            array_push($art_list, $art_data);
        }
        else $i--;
    }
}

echo json_encode($art_list);
?>