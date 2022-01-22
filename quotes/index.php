<?php
    $string = file_get_contents("bartlett_quotes.json");
    $json_a = json_decode($string, true);

    header('Content-Type: application/json; charset=utf-8');
    echo json_encode($json_a);
?>