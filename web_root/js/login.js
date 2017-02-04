// JavaScript Document
//支持Enter键登录
document.onkeydown = function(e) {
    if($(".bac").length == 0) {
        if(!e) e = window.event;
        if((e.keyCode || e.which) == 13) {
            var obtnLogin=document.getElementById("btn_login")
                obtnLogin.focus();
        }
    }
}

$(function() {
    //登录
    $('#btn_login').click(function() {
        show_loading();
        //var usernameReg = /^[a-zA-Z\d]\w{3,11}[a-zA-Z\d]$/;
        var usernameReg = /^(\w)+(\.\w+)*@*(\w)+((\.\w+)*)$/;
        var passwdReg = /^(\w)+(\.\w+)*@*(\w)+((\.\w+)*)$/;
        if($('#username').val() == '') {
            show_err_msg('您的帐号为空！');	
            $('#username').focus();
        } else if(!usernameReg.test($('#username').val())) {
            show_err_msg('您的帐号格式错误！');
            $('#username').focus();
        } else if($('#password').val() == '') {
            show_err_msg('您的密码为空！');
            $('#password').focus();
        } else if(!passwdReg.test($('#password').val())) {
            show_err_msg('您的密码格式错误！'); 
            $('#password').focus();
        } else {
            //ajax提交表单，#login_form为表单的ID。 如：$('#login_form').ajaxSubmit(function(data) { ... });
            var jsObj = {};
            jsObj.username = $('#username').val();
            jsObj.password = $('#password').val();

            $.ajax({
                url: '/SecMng/login',
                type: 'POST',
                data: JSON.stringify(jsObj),
                dataType: 'json',
                success: function(ret) {
                    if(ret.result == 1) {
                        show_msg('登录成功！ 正在为您跳转...','/');
                        window.location.href='../html/admin.html';
                        setCookie(ret.cookiename, ret.id, "s600");
                    } else {
                        show_err_msg('登录失败！用户名或密码错误...');
                    }
                },
                error: function(err) {
                    show_err_msg('登录失败！服务器未响应...');
                }
            });
        }
    });
});

function setCookie(name, value, expiretime) {
    var strsec = getsec(expiretime);
    var exp = new Date();
    exp.setTime(exp.getTime() + strsec * 1);
    document.cookie = name + "=" + escape(value) + 
        ";expires=" + exp.toGMTString() + ";path=SetMng/";
}

function getsec(str) {
    var str1 = str.substring(1, str.length) * 1;
    var str2 = str.substring(0, 1);

    if(str2 == "s")
        return str1 * 1000;
    else if (str2 == "h")
        return str1 * 3600 * 1000;
    else if(str2 == "d")
        return str1 * 24 * 3600 * 1000;
    else
        return 10000;
}

function getCookie(name) {
    var arr, reg = new RegExp("(^| )"+name+"=([^;]*)(;|$)");
    if(arr = document.cookie.match(reg))
        return unescape(arr[2]);
    else
        return null;
}

function delCookie(name) {
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var cval = getCookie(name);
    if(cval != null)
        document.cookie = name + "=" + cval + ";expires=" + 
            exp.toGMTString() + ";path=/";
}
