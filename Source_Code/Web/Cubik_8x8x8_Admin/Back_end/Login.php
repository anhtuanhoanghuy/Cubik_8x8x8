<?php
session_start();
require_once 'Database.php';
$conn = db_connect();

// Kiểm tra AJAX request
$isAjax = isset($_SERVER['HTTP_X_REQUESTED_WITH']) &&
          strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) === 'xmlhttprequest';

// Nếu đã login
if(isset($_SESSION["account"])) {
    if($isAjax){
        echo json_encode(['status'=>1, 'message'=>'Đã đăng nhập']);
        exit;
    } else {
        header("Location: /Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Front_end/HTML/Index.html");
        exit;
    }
}

// Nếu nhận POST login
if(isset($_POST['username'], $_POST['password'])) {
    $username = $_POST['username'];
    $password = md5($_POST['password']);

    $sql = "SELECT * FROM account WHERE user_name = ? AND pass_word = ?";
    $stmt = $conn->prepare($sql);
    $stmt->execute([$username, $password]);

    if($stmt->rowCount() === 1){
        $_SESSION["account"] = $username;
        echo json_encode(['status'=>1,'message'=>'Đăng nhập thành công']);
    } else {
        echo json_encode(['status'=>0,'message'=>'Sai tài khoản hoặc mật khẩu']);
    }

    db_disconnect();
    exit;
}

// Nếu request trực tiếp mà không POST
if(!$isAjax){
    header("Location: /Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Front_end/HTML/Login.php");
    exit;
}
?>
