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
			//提交表单
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
                    $.ajax({
                        url: '/SecMng/login',
                        type: 'GET',
                        data: {username:$('#username').val(), password:$('#password').val()},
                        dataType: 'json',
                        success: function(ret) {
                            if(ret.result == 1) {
                                show_msg('登录成功！ 正在为您跳转...','/');
                                window.location.href='../html/admin.html';
                            } else {
                                show_err_msg('登录失败！用户名或密码错误...');
                            }
                        },
                        error: function(err) {
                            show_err_msg('登录失败！服务器未响应...');
                        }
                    });
                    //show_msg('登录成功！ 正在为您跳转...','/');
				}
			});
		});
