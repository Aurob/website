<?php
header('Content-Type: application/json');



$response = array();

$art_count = 0;

if(isset($_GET['count'])){   
    $art_count = $_GET['count']; 
}

$art_list = [];
if($art_count > 0) {
    
    $art_ids = [];
    // $artists = json_decode(file_get_contents("artists.json"), true);

    $db = new SQLite3('/var/www/html/art/artvee.db');
    $count = $db->querySingle("SELECT COUNT(*) as count FROM art");

    for($i = 0; $i < $art_count; $i++) {
        // $artist = rand(0, count($artists)-1);
        // $rand_art = rand(0, count($artists[$artist]['works'])-1);

        $rand_row = rand(0, $count-1);
        // if(!in_array(strval($artist).strval($rand_art), $art_ids)) {
        if(!in_array($rand_row, $art_ids)) {
            // $art_data = $artists[$artist]['works'][$rand_art];
            // $url = $art_data['image_url'];
            // if(!str_ends_with($url, '.jpg')) {
            //     $i--;
            //     continue;
            // }
            // $img = file_get_contents($url);
            // $b64image = "data:image/jpeg;base64," . base64_encode($img);
            // $art_data['image_b64'] = $b64image;
            // $art_data['size'] = getimagesize($b64image);


            $res = $db->query('SELECT * FROM art WHERE rowid = '.$rand_row);
            while ($row = $res->fetchArray()) {
                $url = $row['link'];
                $img = file_get_contents($url);
                $b64image = "data:image/jpeg;base64," . base64_encode($img);
                $art_data['image_b64'] = $b64image;
                $art_data['size'] = getimagesize($b64image);
            }

            // array_push($art_ids, strval($artist).strval($rand_art));
            array_push($art_ids, $rand_row);
            array_push($art_list, $art_data);
        }
        else $i--;
    }
}
// $url = 
// 'https://www.wga.hu/art/e/eliaerts/bouquet1.jpg'; 
  
// $img = 'logo.png'; 
  
// // Function to write image into file
// $b64image = base64_encode(file_get_contents($url));
echo json_encode($art_list);


?>