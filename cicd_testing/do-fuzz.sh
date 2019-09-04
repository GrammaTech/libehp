#!/bin/bash 

function main()
{
	set -e 
	set -x
	# build software
	git submodule sync --recursive
	git submodule update --recursive --init

	scons -j3
	cd test
	scons 
	cd ..

	# force reinstall tools so we are always up-to-date
	yes | sudo bash -c "$(curl -fsSL allzp.zephyr-software.io/turbo/cli-install.sh)"

	# better done with boost add -q -i 
	turbo-cli boost add libehp || true
	local bid=$(turbo-cli boost list|grep libehp|cut -d"	" -f1)

	turbo-cli seed add $bid cicd_testing/ehp-seed.yaml || true
	local vid=$(turbo-cli version add -q $bid lib/libehp.so)
	turbo-cli fuzz --fuzz-config cicd_testing/afl.yaml --app-config cicd_testing/ehp-config.yaml --ver-id $vid

	local report=$(turbo-cli log get report $vid)

	echo "The report is: "
	echo $report

	local crash_count=$(cat report|shyaml get-value crashing-input-count)

	if [[ $crash_count == 0 ]]; then
		echo "No crashes found"
		exit 0
	else
		echo "$crash_count count crashes found!"
		exit 1
	fi

}

main "$@"
