$(document).ready(function () {

    $("#username").keypress(function (event) {
        if (event.keyCode == 13) {
            event.preventDefault();
            $("#password").focus();
        }
    });

    $("#password").keypress(function (event) {
        if (event.keyCode == 13) {
            event.preventDefault();
            $("#logBttn").click();
        }
    });

    $("#logBttn").click(function (event) {
        if ($("#username").val() == "" || $("#password").val() == "") {
            event.preventDefault();
            alert("Vui lòng nhập đầy đủ thông tin.");
            return;
        }

        var mikExp = /[\$\\#^&*()\[\]+_{}\`~=!|\/?.:;\"\'@]/;
        if (mikExp.test($("#username").val()) || mikExp.test($("#password").val())) {
            event.preventDefault();
            alert("Vui lòng nhập đúng định dạng.");
            return;
        }

        $.post("/Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Back_end/Login.php",
            {
                username: $("#username").val(),
                password: $("#password").val(),
            },
            function (data) {
                try {
                    data = JSON.parse(data);
                } catch (e) {
                    console.error("Response không phải JSON:", data);
                    alert("Server trả về dữ liệu không hợp lệ.");
                    return;
                }

                if (data.status === 1) {
                    window.location.href =
                        "/Cubik_8x8x8/Source_Code/Web/Cubik_8x8x8_Admin/Front_end/HTML/Index.html";
                } else {
                    alert(data.message);
                }
            }
        );
    });
});
