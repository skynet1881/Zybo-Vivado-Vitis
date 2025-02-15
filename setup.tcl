#!/usr/bin/tclsh

proc print_usage {} {
	puts "Usage: setup.tcl <hardware.xsa> <destination>"
	exit 4
}

set index 1
foreach argument $argv {
	set parameter($index) $argument
	incr index 1
}

# setting the parsed arguments
if { [catch {set system_xsa_location_arg $parameter(1)} fid] } {
	print_usage
}
if { [catch {set project_location $parameter(2)} fid] } {
	print_usage
}

set script_path [ file dirname [ file normalize [ info script ] ] ]

# check if the xsa exists
if {[file exists $system_xsa_location_arg]} {
	set system_xsa_location [file join [pwd] $system_xsa_location_arg]
	puts "xsa file used: $system_xsa_location"
} else {
	puts "No such xsa file: $system_xsa_location_arg"
	exit 2
}

setws $project_location

platform create -no-boot-bsp -name zybo_z7_10 -hw $system_xsa_location
#domain create -name app_domain -os standalone -proc psu_cortexr5_0
domain create -name app_domain -os standalone -proc ps7_cortexa9_0
app create -name sw -domain app_domain -template "Empty Application(C)"
importsources -name sw -path $script_path/src/
#importsources -name sw -target-path ./common-librpu -path $script_path/../common-librpu/
#file copy -force -- $script_path/src/lscript.ld $project_location/sw/src/lscript.ld

cd $project_location
#exec patch -p1 -i $script_path/patches/bsp.patch

#app config -name sw -add include-path $project_location/sw/common-librpu
app build -name sw
#sysproj build -name sw_system

exit
