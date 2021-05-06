.section .rodata
    .global settings_root
	.type   settings_root, @object
	.align  4
    .global test_menu
	.type   test_menu, @object
	.align  4
    .global external_hdd
	.type   external_hdd, @object
	.align  4

settings_root:
    .incbin "settings_root.xml"
settings_root_End:
    .global settings_root_Size
    .type   settings_root_Size, @object
    .align  4
settings_root_Size:
    .int    settings_root_End - settings_root

test_menu:
    .incbin "test_menu.xml"
test_menu_End:
    .global test_menu_Size
    .type   test_menu_Size, @object
    .align  4
test_menu_Size:
    .int    test_menu_End - test_menu

external_hdd:
    .incbin "external_hdd.xml"
external_hdd_End:
    .global external_hdd_Size
    .type   external_hdd_Size, @object
    .align  4
external_hdd_Size:
    .int    external_hdd_End - external_hdd