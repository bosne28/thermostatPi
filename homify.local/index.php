<?php
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['temperature'])) {
    $temperature = $_POST['temperature'];
    $file = 'reqtemp';

    // Write the temperature to the file
    file_put_contents($file, $temperature);

    // Respond with a success message
    echo 'Temperature saved: ' . $temperature;
} else {
    // Respond with an error message
    echo 'Invalid request';
}
?>