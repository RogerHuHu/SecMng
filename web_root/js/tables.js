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

var $tableClone;
var $rowClone;
var addFlag = 0;

$(function() {
    //新增账号
    $('#btn-add-account').click(function() {
        $rowClone = $('#tbody-test').find('tr.hide').clone(true).removeClass("hide odd");
        $('#modal-account').modal('show');
        addFlag = 1;
    });
});

$("#btn-edit").click(function() {
    $tableClone = $(this).parents('tr').children('td');
    $('#modal-text-target').val($tableClone.get(0).innerText);
    $('#modal-text-username').val($tableClone.get(1).innerText);
    $('#modal-text-password').val($tableClone.get(2).innerText);
    $('#modal-account').modal('show');
});

$('#modal-btn-check').click(function() {
    if(addFlag == 0) {
        $tableClone.get(0).innerText = $('#modal-text-target').val().trim();
        $tableClone.get(1).innerText = $('#modal-text-username').val().trim();
        $tableClone.get(2).innerText = $('#modal-text-password').val().trim();
    } else if(addFlag == 1) {
        $rowClone.children('td').get(0).innerText = $('#modal-text-target').val().trim();
        $rowClone.children('td').get(1).innerText = $('#modal-text-username').val().trim();
        $rowClone.children('td').get(2).innerText = $('#modal-text-password').val().trim();
        $('#tbody-test').prepend($rowClone);
        addFlag = 0;
    }
    $('#modal-account').modal('hide');
});

$("#btn-check").click(function() {
    if(confirm("是否保存")) {
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
    $tableClone = $(this).parents('tr').children('td');
    $('#modal-password-rule').modal('show');
    $modal.modal('show');
});

$('#modal-password-rule-btn-check').click(function() {
    var length = $('#modal-text-length').val();
    var type = $("input[name='option-radio-inline']:checked").val();
    $tableClone.get(2).innerText = GeneratePassword(parseInt(length), parseInt(type));
    $('#modal-password-rule').modal('hide');
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
