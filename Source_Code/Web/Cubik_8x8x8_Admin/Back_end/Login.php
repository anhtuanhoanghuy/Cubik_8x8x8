<?php
session_start();
require_once 'Database.php'; // file chứa db_connect()

$conn = db_connect();

// Nếu đã đăng nhập thì chuyển trang
if (isset($_SESSION["account"])) {
    header("Location: /Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Front_end/HTML/Index.html");
    exit;
}

// Kiểm tra nếu nhận POST
if (isset($_POST['username']) && isset($_POST['password'])) {

    $username = $_POST['username'];
    $password = md5($_POST['password']);

    $sql = "SELECT * FROM account WHERE user_name = ? AND pass_word = ?";
    $stmt = $conn->prepare($sql);
    $stmt->execute([$username, $password]);

    if ($stmt->rowCount() === 1) {

        // Lưu session đăng nhập
        $_SESSION["account"] = $username;

        echo json_encode([
            'status'  => 1,
            'message' => 'Đăng nhập thành công'
        ]);
    } else {
        echo json_encode([
            'status'  => 0,
            'message' => 'Sai tài khoản hoặc mật khẩu'
        ]);
    }

    // Đóng kết nối
    db_disconnect();
    exit;
}
?>
