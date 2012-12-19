
$('#communitytopics').columnize({ width: 230, buildOnce: true, lastNeverTallest: true, doneFunc: function() {$('#communitytopics').hide();}});

$('#coretopics').columnize({ width: 230, buildOnce: true, lastNeverTallest: true});

$('#coretopics .coretopic input').click(function () {
    if ($('[id=SelectCategoryList_' + this.value + ']').attr('checked') != this.checked) {
        $checkboxSpan = $('[id=SelectCategoryList_' + this.value + ']').parent().find("span");
        if (this.checked) {
            $checkboxSpan.addClass('ui-icon-check');
            $checkboxSpan.removeClass('ui-icon-empty');
        } else {
            $checkboxSpan.addClass('ui-icon-empty');
            $checkboxSpan.removeClass('ui-icon-check');
        }
        $('[id=SelectCategoryList_' + this.value + ']').attr('checked', this.checked);
        $('[id=SelectCategoryList_' + this.value + ']').attr('aria-checked', this.checked);
    }
});

$('#communitytopics .coretopic input').click(function () {
    if ($('[id=SelectCoreCategoryList_' + this.value + ']').attr('checked') != this.checked) {
        $checkboxSpan = $('[id=SelectCoreCategoryList_' + this.value + ']').parent().find("span");
        if (this.checked) {
            $checkboxSpan.addClass('ui-icon-check');
            $checkboxSpan.removeClass('ui-icon-empty');
        } else {
            $checkboxSpan.addClass('ui-icon-empty');
            $checkboxSpan.removeClass('ui-icon-check');
        }
        $('[id=SelectCoreCategoryList_' + this.value + ']').attr('checked', this.checked);
        $('[id=SelectCoreCategoryList_' + this.value + ']').attr('aria-checked', this.checked);
    }
});

$("#showcommunitytopics").click(function () {
    $('#coretopics').slideUp(500, function() {
        $('#communitytopics').slideDown(500)
    });
});

$("#hidecommunitytopics").click(function () {
    $('#communitytopics').slideUp(500, function() {
        $('#coretopics').slideDown(500)
    });
});
