Import-Module VirtualMachineManager | Out-Null;
$template = Get-SCVMTemplate -Name '{{ scvmm_image }}' -VMMServer localhost;
$vmconfig = New-SCVMConfiguration -VMTemplate $template -Name 'ManageIQConfig-{{ vm_name }}';
$vmhost   = Get-SCVMHost -ComputerName '{{ scvmm_host }}';
Set-SCVMConfiguration -VMConfiguration $vmconfig -VMHost $vmhost -VMLocation '{{ scvmm_datastore }}' | Out-Null;
Update-SCVMConfiguration -VMConfiguration $vmconfig | Out-Null;
$vm = New-SCVirtualMachine -Name '{{ vm_name }}' -VMConfiguration $vmconfig;
Set-SCVirtualMachine -VM $vm -CPUCount 1 -DynamicMemoryEnabled $false -MemoryMB 1024 | Out-Null;
$adapter = $vm | SCVirtualNetworkAdapter;
Set-SCVirtualNetworkAdapter -VirtualNetworkAdapter $adapter -LogicalNetwork (Get-SCLogicalNetwork -Name '{{ scvmm_network }}') | Out-Null;
$vm | Select-Object ID | ConvertTo-Json -Compress