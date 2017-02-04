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
        },
        "processing": true,
        "serverSide": true,

        "sAjaxSource": "/SecMng/GetSecrets",
        "fnServerData": function(reqUrl, aoData, fnCallback, settings) {
            GetSecrets(reqUrl, aoData, fnCallback, settings);
        },

        "columns": [
            {"data": "target"},
            {"data": "username"},
            {"data": "password"},
            {"data": null}
        ],

        "columnDefs": [
            {
                "targets": 3,
                "defaultContent": "<button type='button' id='btn-edit' class='btn btn-success btn-circle'><i class='fa fa-edit'></i></button>" + 
                                  "<button type='button' id='btn-check' class='btn btn-info btn-circle' disabled='true'><i class='fa fa-check'></i></button>" +
                                  "<button type='button' id='btn-refresh' class='btn btn-primary btn-circle'><i class='fa fa-refresh'></i></button>" +
                                  "<button type='button' id='btn-del' class='btn btn-danger btn-circle'><i class='fa fa-trash-o'></i></button>"
            }
        ]
    });
});

function GetSecrets(reqUrl, aoData, fnCallback, settings) {
    $.ajax({
        "url": reqUrl,
        "type": "POST",
        "data": aoData,
        "dataType": "json",
        "success": function(ret) {
            if(ret.result == 0) {
                alert("请重新登录！");
                window.location.href = "../html/login.html";
            } else if(ret.result == 1) {
                fnCallback(ret);
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
            addFlag = 1;
            $('#modal-account').modal('show');
        } else {
            alert("请先保存之前的更改");
        }
    });
});

$('#modal-btn-check').click(function() {
    var table = $('#dataTables-example').DataTable();
    if(confirm("是否保存")) {
        var jsObj = {};
        jsObj.type = (addFlag == 0) ? 1 : 0;
        jsObj.target = $('#modal-text-target').val().trim();
        jsObj.username = $('#modal-text-username').val().trim();
        jsObj.password = $('#modal-text-password').val().trim();

        $.ajax({
            url: '/SecMng/SaveSecrets',
            type: 'POST',
            data: JSON.stringify(jsObj),
            dataType: 'json',
            success: function(ret) {
                addFlag = 0;
                if(ret.result == 1) {
                    editFlag = 0;
                    alert("保存成功");
                    $('#dataTables-example').DataTable().ajax.reload();
                } else {
                    alert("请重新登录！");
                    window.location.href = "../html/login.html";
                }
            },
        });
    }

    $('#modal-account').modal('hide');
});

$('#dataTables-example').on('click', 'tbody tr td #btn-edit', function() {
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

$('#dataTables-example').on('click', 'tbody tr td #btn-check', function() {
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
                    $('#dataTables-example').DataTable().ajax.reload();
                } else {
                    alert("请重新登录！");
                    window.location.href = "../html/login.html";
                }
            },
        });
    }
});

$('#dataTables-example').on('click', 'tbody tr td #btn-refresh', function() {
    $rowClone = $(this).parents('tr');
    $tableClone = $rowClone.children('td');
    $('#modal-password-rule').modal('show');
    $modal.modal('show');
});

$('#dataTables-example').on('click', 'tbody tr td #btn-del', function() {
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
                    $('#dataTables-example').DataTable().ajax.reload();
                } else {
                    alert("请重新登录！");
                    window.location.href = "../html/login.html";
                }
            },
        });
    }
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
