package logic

import (
	"flag"
	"fmt"

	"github.com/streadway/amqp"
)

/// 消息生产者接收客户端消息 并整理发送到服务器
type RabProductMsq struct {
	Uri          string
	ExchangeName string
	ExchangeType string
	RoutingKey   string
	Body         string
	Reliable     bool

	Conn    *amqp.Connection
	Channel *amqp.Channel

	Msgs []string
}

var productMsgInstance *RabProductMsq

func GetProductMsgInstance() *RabProductMsq {
	if nil == productMsgInstance {
		productMsgInstance = &RabProductMsq{}
		productMsgInstance.Init()
	}
	return productMsgInstance
}

func (f *RabProductMsq) Init() {

	fmt.Printf("Init RabproductMsq ...")
	//初始化消息队列数据
	f.Msgs = make([]string, 0)

	f.Uri = "amqp://liuzilun:x@192.168.1.105:5672/"
	f.ExchangeName = "wething"
	f.ExchangeType = "direct"
	f.RoutingKey = "wething-key"
	f.Body = "foobar"
	f.Reliable = true

	flag.Parse()
	// 链接RabbitMQ
	var err error
	f.Conn, err = amqp.Dial("amqp://liuzilun:x@192.168.1.105:5672/")
	if err != nil {
		fmt.Printf("Failed to connect to RabbitMQ,err:%v", err)
	}

	f.Channel, err = f.Conn.Channel()
	if err != nil {
		fmt.Printf("Failed to Open Channel err:%v", err)
	}
	//fmt.Printf("init RabProductMsq init! Channel:%v  Conn:%v", f.Channel, f.Conn)
}

//接收来自外部消息
func (f *RabProductMsq) SetMsg(msg string) {
	//fmt.Printf("SetMsg  msg cont :%v ", msg)
	f.Msgs = append(f.Msgs, msg)
	//fmt.Printf("f.Msgs len(Msgs)  %v)\n", len(f.Msgs))
}

// 发布消息
func (f *RabProductMsq) PublishMsg() error {
	//fmt.Printf("PublishMsgss  %v chanel is %v ) \n", f.Msgs, f.Channel)
	if err := f.Channel.ExchangeDeclare(
		f.ExchangeName, // name
		f.ExchangeType, // type
		true,           // durable
		false,          // auto-deleted
		false,          // internal
		false,          // noWait
		nil,            // arguments
	); err != nil {
		fmt.Printf("Exchange Declare: %s", err)
		return fmt.Errorf("Exchange Declare: %s", err)
	}

	// Reliable publisher confirms require confirm.select support from the
	// connection.
	if f.Reliable {
		fmt.Printf("enabling publishing confirms.")
		if err := f.Channel.Confirm(false); err != nil {
			return fmt.Errorf("Channel could not be put into confirm mode: %s", err)
		}

		confirms := f.Channel.NotifyPublish(make(chan amqp.Confirmation, 1))

		defer confirmOne(confirms)
	}

	//fmt.Printf("declared Exchange, publishing %dB body (%q)", len(body), body)
	for key, v := range f.Msgs {
		fmt.Printf("for range key = %d  msg v:=  %v)\n", key, v)
		if err := f.Channel.Publish(
			f.ExchangeName, // publish to an exchange
			f.RoutingKey,   // routing to 0 or more queues
			false,          // mandatory
			false,          // immediate
			amqp.Publishing{
				Headers:         amqp.Table{},
				ContentType:     "text/plain",
				ContentEncoding: "",
				Body:            []byte(v),
				DeliveryMode:    amqp.Transient, // 1=non-persistent, 2=persistent
				Priority:        0,              // 0-9
				// a bunch of application/implementation-specific fields
			},
		); err != nil {
			fmt.Printf("Exchange Publish: msg%s  err:%s", v, err)
			continue
		}
		fmt.Printf("[Publishing]PublishMsg  %v)\n", v)
	}
	return nil
}

func confirmOne(confirms <-chan amqp.Confirmation) {
	fmt.Printf("waiting for confirmation of one publishing")

	if confirmed := <-confirms; confirmed.Ack {
		fmt.Printf("confirmed delivery with delivery tag: %d", confirmed.DeliveryTag)
	} else {
		fmt.Printf("failed delivery of delivery tag: %d", confirmed.DeliveryTag)
	}
}
