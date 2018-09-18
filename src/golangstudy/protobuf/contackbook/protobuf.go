package main

import(
    "github.com/golang/protobuf/proto"
     "grpc-src/protobuf"
     "fmt"
     "io/ioutil"
)


func writeProto(filenae string)(err error){
    var contactBook address.ContactBook


    for m:= 0 ;m<50;m++ {
        p:=&address.Person{
            Id:  int32(m),
            Name: fmt.Sprintf("刘红%d",m),
        }
        phone:=&address.Phone{
            Type:address.PhoneType_HOME,
            Number:"17727961386",
        }
        p.Phones = append(p.Phones,phone)
        contactBook.Persons = append(contactBook.Persons,p)
    }

    data,err2:= proto.Marshal(&contactBook)
    if err2 != nil{
        fmt.Printf("marshal proto buf failed,err:%v",err2)
        return
    }
    err3 := ioutil.WriteFile(filenae,data,0755)
    if err3!= nil{
        fmt.Printf("writefile to file failed %v \n",err3)
        return
    }
    return 
}


func readProto(finlename string )(err error){
    var contactBook address.ContactBook 

    data,err := ioutil.ReadFile(finlename)

    err = proto.Unmarshal(data,&contactBook)

    fmt.Printf("protoc:%#v\n",contactBook)
    
    return
}
func main(){
    filename:="/home/liuzilun/ontactbook.dat"
    writeProto(filename)

    readProto(filename)

}