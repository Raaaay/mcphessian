## mcphessian
======
One PHP extension of Hessian Client & Server based on mc_pcak.
###Requirement
mc_pack extension.
###Example
```
// Assuming we already have a service which returns the data received.
$data = array(123456);
$obj = new McpackHessianClient('http://cas.baidu.com/services/ApiService.php');
$ret = $obj->getExample($data);
var_dump($ret)
// print Array(123456)
```
