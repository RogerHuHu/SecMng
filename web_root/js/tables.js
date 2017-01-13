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
    var $clone = $(this).parents('tr').children('td');
    $('#modal-text-target').val($clone.get(0).innerText);
    $('#modal-text-username').val($clone.get(1).innerText);
    $('#modal-text-password').val($clone.get(2).innerText);
    $('#modal-account').modal('show');

    $('#modal-btn-check').click(function() {
        $clone.get(0).innerText = $('#modal-text-target').val().trim();
        $clone.get(1).innerText = $('#modal-text-username').val().trim();
        $clone.get(2).innerText = $('#modal-text-password').val().trim();
        $('#modal-account').modal('hide');
    });
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

$("#btn-refresh").click(function() {
    var $clone = $(this).parents('tr').children('td');
    $('#modal-password-rule').modal('show');

    $('#modal-password-rule-btn-check').click(function() {
        var length = $('#modal-text-length').val();
        if($('#option-radio-pure').attr("checked"))
            $clone.get(2).innerText = GeneratePassword(parseInt(length), 0)
        else if($('#option-radio-mix1').attr("checked"))
            $clone.get(2).innerText = GeneratePassword(parseInt(length), 1)
        else if($('#option-radio-mix2').attr("checked"))
            $clone.get(2).innerText = GeneratePassword(parseInt(length), 2)
        $('#modal-password-rule').modal('hide');
    });
    /*
    if(confirm("是否更新密码")) {
        var length = prompt("请输入密码长度", ""); 
        if(length != null && length != "") {
            $(this).parents('tr').children("td").get(2).innerText = GeneratePassword(parseInt(length));
        }
    }
    */
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

function GeneratePassword(length, special) {
    /*
    var iteration = 0;
    var password = "";
    var randomNumber;
    if(special == undefined){
        var special = false;
    }
    while(iteration < length){
        randomNumber = (Math.floor((Math.random() * 100)) % 94) + 33;
        if(!special){
            if ((randomNumber >=33) && (randomNumber <=47)) { continue; }
            if ((randomNumber >=58) && (randomNumber <=64)) { continue; }
            if ((randomNumber >=91) && (randomNumber <=96)) { continue; }
            if ((randomNumber >=123) && (randomNumber <=126)) { continue; }
        }
        iteration++;
        password += String.fromCharCode(randomNumber);
    }
    return password;
    */

    if(special == undefined)
        var special = 0;
    if(special == 0) {
        var secret="0123456789";
    } else if(special == 1) {
        var secret="abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    } else if(special == 2) {
        var secret="abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~!-_?*&^%#@()";
    }

    var selen = secret.length - 1;
    var i = 0;
    var password = "";
    while(i < length) {
        password += secret[Math.round(Math.random() * selen)];
        i++;    
    }
    return password;
}

String.prototype.trim = function() {
    return this.replace(/(^\s*)|(\s*$)/g, "");
}
