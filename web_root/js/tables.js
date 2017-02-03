$(document).ready(function() {
    $('#dataTables-example').DataTable({
        responsive: true,
        "oLanguage": {
        "sLengthMenu": "每页显示 _MENU_ 条记录",
        "sZeroRecords": "抱歉， 没有找到",
        "sInfo": "从 _START_ 到 _END_ /共 _TOTAL_ 条数据",
        "sInfoEmpty": "没有数据",
        "sInfoFiltered": "(从 _MAX_ 条数据中检索)",
        "oPaginate": {
            "sFirst": "首页",
            "sPrevious": "前一页",
            "sNext": "后一页",
            "sLast": "尾页"
            }
        }
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
                    $clone.children('td').eq(3).children('button').eq(1).attr("disabled", "true");
                    $('#tbody-test').append($clone);
                }
            }
        }
    });
}

var $tableClone;
var $rowClone;
var addFlag = 0;
var saveFlag = 0;
var editFlag = 0;

$(function() {
    //新增账号
    $('#btn-add-account').click(function() {
        if(editFlag == 0) {
            $rowClone = $('#tbody-test').find('tr.hide').clone(true).removeClass("hide odd");
            $tableClone = $rowClone.children('td');
            $('#modal-account').modal('show');
            addFlag = 1;
        } else {
            alert("请先保存之前的更改");
        }
    });
});

$("#btn-edit").click(function() {
    if(editFlag == 0) {
        $rowClone = $(this).parents('tr');
        $tableClone = $(this).parents('tr').children('td');
        $('#modal-text-target').val($tableClone.get(0).innerText);
        $('#modal-text-username').val($tableClone.get(1).innerText);
        $('#modal-text-password').val($tableClone.get(2).innerText);
        $('#modal-account').modal('show');
    } else {
        alert("请先保存之前的更改");
    }
});

$('#modal-btn-check').click(function() {
//    editFlag = 1;
    $tableClone.eq(3).children('button').eq(1).removeAttr("disabled");
    if(addFlag == 0) {
        $tableClone.get(0).innerText = $('#modal-text-target').val().trim();
        $tableClone.get(1).innerText = $('#modal-text-username').val().trim();
        $tableClone.get(2).innerText = $('#modal-text-password').val().trim();
        $rowClone.attr("opttype", "1");
        saveFlag = 1;
    } else if(addFlag == 1) {
        $rowClone.children('td').get(0).innerText = $('#modal-text-target').val().trim();
        $rowClone.children('td').get(1).innerText = $('#modal-text-username').val().trim();
        $rowClone.children('td').get(2).innerText = $('#modal-text-password').val().trim();
        $rowClone.attr("opttype", "0");
        $('#tbody-test').prepend($rowClone);
        addFlag = 0;
        saveFlag = 0;
    }
    $('#modal-account').modal('hide');
});

$("#btn-check").click(function() {
    if(confirm("是否保存")) {
        var jsObj = {};
        var tdObj = $(this).parents('tr').children('td');
        jsObj.type = parseInt($(this).parents('tr').attr("opttype"));
        jsObj.target = tdObj.get(0).innerText;
        jsObj.username = tdObj.get(1).innerText;
        jsObj.password = tdObj.get(2).innerText;

        $.ajax({
            url: '/SecMng/SaveSecrets',
            type: 'POST',
            data: JSON.stringify(jsObj),
            dataType: 'json',
            success: function(ret) {
                if(ret.result == 1) {
                    tdObj.eq(3).children('button').eq(1).attr("disabled", "true");
                    editFlag = 0;
                    alert("保存成功");
                } else {
                    alert("保存失败");
                }
            },
        });
    }
});


$("#btn-refresh").click(function() {
    $rowClone = $(this).parents('tr');
    $tableClone = $rowClone.children('td');
    $('#modal-password-rule').modal('show');
    $modal.modal('show');
});

$('#modal-password-rule-btn-check').click(function() {
    var length = $('#modal-text-length').val();
    var type = $("input[name='option-radio-inline']:checked").val();
    if($rowClone.attr("opttype") != "0")
        $rowClone.attr("opttype", "1");
    $tableClone.eq(3).children('button').eq(1).removeAttr("disabled");
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
