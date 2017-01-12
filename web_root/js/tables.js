$(document).ready(function() {
    $('#dataTables-example').DataTable({
        responsive: true
    });

    GetSecrets();
});

function GetSecrets() {
    $.ajax({
        url: '/SecMng/GetSecrets',
        type: 'POST',
        dataType: 'json',
        success: function(ret) {
            if(ret.result == 0) {
                alert("重新请登录!");
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
        var jsObj = {};
        jsObj.target = $(this).parents('tr').children("td").get(0).innerText;
        jsObj.username = $(this).parents('tr').children("td").get(1).innerText;
        jsObj.password = $(this).parents('tr').children("td").get(2).innerText;

        $.ajax({
            url: '/SecMng/SaveSecrets',
            type: 'POST',
            data: JSON.stringify(jsObj),
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
        var $clone = $(this).parents('tr');
        var jsObj = {};
        jsObj.target = $clone.children("td").get(0).innerText;
        jsObj.username = $clone.children("td").get(1).innerText;
        jsObj.password = $clone.children("td").get(2).innerText;

        $.ajax({
            url: '/SecMng/DelSecrets',
            type: 'POST',
            data: JSON.stringify(jsObj),
            dataType: 'json',
            success: function(ret) {
                if(ret.result == 1) {
                    $clone.detach();
                }
            },
        });
    }
});
