$(document).ready(function() {
    $('#dataTables-example').DataTable({
        responsive: true
    });

    GetSecrets();
});

function GetSecrets() {
    $.ajax({
        url: '/SecMng/GetSecrets',
        type: 'GET',
        dataType: 'json',
        success: function(ret) {
            if(ret.result == 0) {
                alert("请登录!");
                window.location.href='../html/login.html';
            } else if(ret.result == 1){
                for(var i = 0; i < ret.accounts.length; i++) {
                    var $clone = $('#tbody-test').find('tr.hide').clone(true).removeClass("hide odd");
                    $clone.children("td").get(0).innerHTML = ret.accounts[i].target;
                    $clone.children("td").get(1).innerHTML = ret.accounts[i].username;
                    $clone.children("td").get(2).innerHTML = ret.accounts[i].password;
                    $('#tbody-test').append($clone);
                }
            }
        }
    });
}

$(function() {
    //新增账号
    $('#btn-add-account').click(function() {
        var $clone = $('#tbody-test').find('tr.hide').clone(true).removeClass("hide odd");
        $('#tbody-test').prepend($clone);
    });
});

$("#btn-edit").click(function() {
    if(confirm("是否修改")) {
        $(this).parents('tr').find("td[contenteditable='false']").attr("contenteditable", "true");
    }
});

$("#btn-check").click(function() {
    if(confirm("是否保存")) {
        $(this).parents('tr').find("td[contenteditable='true']").attr("contenteditable", "false");
        var strTarget = $(this).parents('tr').children("td").get(0).innerText;
        var strUsername = $(this).parents('tr').children("td").get(1).innerText;
        var strPassword = $(this).parents('tr').children("td").get(2).innerText;
        $.ajax({
            url: '/SecMng/SaveSecrets',
            type: 'GET',
            data: {target:strTarget, username:strUsername, password:strPassword},
            contentType: 'application/x-www-form-urlencoded; charset=UTF-8',
            dataType: 'json',
            success: function(ret) {
                if(ret.result == 1) {
                    alert("保存成功");
                } else {
                    alert("保存失败");
                }
            },
        });
    }
});

$("#btn-del").click(function() {
    if(confirm("是否删除")) {
        $(this).parents('tr').detach();
    }
});
