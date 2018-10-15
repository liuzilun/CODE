package msgserver

import (
	"encoding/json"
	"fmt"

	//"time"

	"wething/model"

	"github.com/streadway/amqp"
)

//处理解析消息并存入DBServer
type MqMsqHand struct {
	Uri          string
	ExchangeName string
	ExchangeType string
	BindingKey   string
	QueueName    string
	ConsumerTag  string

	Conn    *amqp.Connection
	Channel *amqp.Channel
	Queue   amqp.Queue

	Done chan error

	Msgs []string
}

//var mqMsgHandInstance *MqMsqHand = nil

//func GetMqMsgHandInstance() *MqMsqHand {
//	if mqMsgHandInstance == nil {
//		mqMsgHandInstance = &MqMsqHand{}
//		mqMsgHandInstance.Init()
//	}
//	return mqMsgHandInstance
//}

func (f *MqMsqHand) Init() {

	f.Conn = nil
	f.Channel = nil
	f.Queue = amqp.Queue{}
	f.Done = make(chan error)

	f.Uri = "amqp://liuzilun:x@192.168.1.105:5672/"
	f.ExchangeName = "wething"
	f.ExchangeType = "direct"
	f.BindingKey = "wething-key"

	f.QueueName = "wething-queue"
	f.ConsumerTag = "simple-consumer"

	fmt.Printf("dialing %q\n", f.Uri)
	var err error
	f.Conn, err = amqp.Dial(f.Uri)
	if err != nil {

		fmt.Printf("init msgQuere failed，Dial: %s\n", err)
		return
	}

	//	go func() {
	//		fmt.Printf("closing: %s\n", <-f.Conn.NotifyClose(make(chan *amqp.Error)))
	//	}()

	//fmt.Printf("got Connection, getting Channel f.Conn:%s \n", f.Conn)
	f.Channel, err = f.Conn.Channel()
	if err != nil {
		fmt.Printf("init MsgHand failed,err:%v \n", err)
		return
	}

	//fmt.Printf("got Channel, declaring Exchange (%q)\n,f.Channel:%s\n", f.ExchangeName, f.Channel)
	if err = f.Channel.ExchangeDeclare(
		f.ExchangeName, // name of the exchange
		f.ExchangeType, // type
		true,           // durable
		false,          // delete when complete
		false,          // internal
		false,          // noWait
		nil,            // arguments
	); err != nil {
		fmt.Printf("init MsgHand failed,ExchangeDeclare failled err:%v\n ", err)
		return
	}

	fmt.Printf("declared Exchange, declaring Queue %q\n", f.QueueName)

	f.Queue, err = f.Channel.QueueDeclare(
		f.QueueName, // name of the queue
		true,        // durable
		false,       // delete when unused
		false,       // exclusive
		false,       // noWait
		nil,         // arguments
	)
	if err != nil {
		fmt.Printf("init MsgHand failed,QueueDeclare failled err:%v \n", err)
		return
	}

	fmt.Printf("declared Queue (%q %d messages, %d consumers), binding to Exchange (key %q) exchangeName(%q)\n",
		f.Queue.Name, f.Queue.Messages, f.Queue.Consumers, f.BindingKey, f.ExchangeName)

	if err = f.Channel.QueueBind(
		f.Queue.Name,   // name of the queue
		f.BindingKey,   // bindingKey
		f.ExchangeName, // sourceExchange
		false,          // noWait
		nil,            // arguments
	); err != nil {
		fmt.Printf("init MsgHand failed,QueueBind failled err:%v \n", err)
		return
	}

	fmt.Printf("MsgConsume: init end f.Channel:%v,f.QueueName:%v consumerTag:%v\n\n", f.Channel, f.QueueName, f.ConsumerTag)
	//fmt.Printf("-------------------------------------------------------------------\nn")
}

func (f *MqMsqHand) MsgConsume() {
	fmt.Printf("--------------------------------------------------------------------------\n")
	fmt.Printf("MsgConsume: begin to consume f.Channel:%v,f.QueueName:%v consumerTag:%v",
		f.Channel, f.Queue.Name, f.ConsumerTag)
	deliveries, err := f.Channel.Consume(
		f.Queue.Name,  // name
		f.ConsumerTag, // consumerTag,
		false,         // noAck
		false,         // exclusive
		false,         // noLocal
		false,         // noWait
		nil,           // arguments
	)
	if err != nil {
		fmt.Printf("Queue Consume error : %s", err)
		return
	}
	fmt.Printf("got %dB delivery: ", len(deliveries))
	fmt.Printf("--------------------------------------------------------------------------\n")

	//go handle(deliveries, f.Done)

	go HandUserMsg(deliveries, f.Done)

	//fmt.Printf("after handle got %dB delivery: ", len(deliveries))
	//}

}

func (f *MqMsqHand) Shutdown() error {
	// will close() the deliveries channel
	if err := f.Channel.Cancel(f.ConsumerTag, true); err != nil {
		return err
	}

	if err := f.Conn.Close(); err != nil {
		return fmt.Errorf("AMQP connection close error: %s", err)
	}

	defer fmt.Printf("AMQP shutdown OK")

	// wait for handle() to exit
	return <-f.Done
}

func HandUserMsg(deliveries <-chan amqp.Delivery, done chan error) {
	//消息服务器，处理消息逻辑 ，解消息，缓存，用户逻辑，业务逻辑都在这里处理，
	//处理完然后存数据库
	fmt.Printf("HandUserMsg: begin to handle msg\n")
	msginfo := &model.WeThingDetail{}
	for d := range deliveries {
		err := json.Unmarshal([]byte(d.Body), msginfo)
		if err != nil {
			fmt.Printf("json marshal failed\n")
			//消息处理失败，需要处理
			return
		}
		PutMsgtoDbServer(msginfo)
		d.Ack(false)
	}
	done <- nil
}
func PutMsgtoDbServer(msg *model.WeThingDetail) {
	//log.Printf("handle: deliveries channel closed")
	//将消息序列化json 传到DBServer
	data, err := json.Marshal(msg)
	if err != nil {
		fmt.Println("json marshal failed")
		return
	}
	RPCHandleMsgToDbserver(string(data))
}
