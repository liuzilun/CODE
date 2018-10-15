package main

import (
    "log"
    "github.com/streadway/amqp"
)

// 用于报错处理
func failOnError(err error, msg string) {
    if err != nil {
        log.Fatalf("%s: %s", msg, err)
    }
}

func main() {
    // 链接RabbitMQ
    conn, err := amqp.Dial("amqp://liuzilun:x@192.168.1.105:5672/")
    failOnError(err, "Failed to connect to RabbitMQ")
    defer conn.Close()

    // 打开Channel
    ch, err := conn.Channel()
    failOnError(err, "Failed to open a channel")
    defer ch.Close()

    // 声明Queue
    q, err := ch.QueueDeclare(
        "hello", // 队列名
        false,   // 持久性
        false,   // 不用时是否删除队列
        false,   // exclusive
        false,   // no-wait
        nil,     // arguments
    )
    failOnError(err, "Failed to declare a queue")

    // 需要发送的文本
    body := "hello"
    // 发布消息
    err = ch.Publish(
        "",     // 交换机名称
        q.Name, // 路由名称
        false,  // mandatory
        false,  // immediate
        amqp.Publishing{
            ContentType: "text/plain", // 文本格式
            Body:        []byte(body), // 内容
        })
    log.Printf(" [x] Sent %s", body)
    failOnError(err, "Failed to publish a message")
}
