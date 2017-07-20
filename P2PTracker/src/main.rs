enum TrkCom {
    ADD,
    REMOVE,
    LIST,
}

fn p2p_tracker(first : std::net::SocketAddrV4) {
    if let Ok(tracker) = std::net::UdpSocket::bind((std::net::Ipv4Addr::from(0), 0)) {
        if let Ok(_) = tracker.set_read_timeout(Some(std::time::Duration::new(1, 0))) {
            let mut connection_is_good = false;
            for i in 0..4 {
                if let Ok(_) = tracker.send_to(&[0; 0], first) {
                    if let Ok((_, std::net::SocketAddr::V4(first_secure))) = tracker.recv_from(&mut [0; 0]) {
                        if first_secure != first {
                            println!("insecure");
                            return
                        }
                        connection_is_good = true;
                        break
                    } else {
                        println!("failed {}", i);
                    }
                }
            }
            if connection_is_good {
                println!("good");
                let mut clients = vec![first];
                let mut i = 0usize;
                loop {
                    let mut com = TrkCom::LIST; // random - rust warns uninit, com is an output variable
                    if clients.len() < 33 {
                        i = 0;
                    }
                    if let Ok((_, std::net::SocketAddr::V4(client))) = unsafe {
                        tracker.recv_from(
                            std::slice::from_raw_parts_mut(
                                &mut com as *mut _ as *mut u8, std::mem::size_of::<TrkCom>()
                            )
                        )
                    } {
                        match com {
                            TrkCom::LIST => {
                                if let Err(_) = tracker.send_to(
                                    unsafe {
                                        std::slice::from_raw_parts(
                                            clients.as_ptr().offset(i as isize) as *const _ as *const u8,
                                            std::cmp::min(32, clients.len()) * std::mem::size_of::<std::net::SocketAddrV4>()
                                        )
                                    },
                                    client
                                ) {
                                    println!("TrkCom::LIST send_to err");
                                } else {
                                    println!("TrkCom::LIST send_to ok");
                                }
                            }
                            TrkCom::ADD => {
                                clients.push(client);
                                if let Err(_) = tracker.send_to(
                                    unsafe {
                                        std::slice::from_raw_parts(
                                            &com as *const _ as *mut u8, std::mem::size_of::<TrkCom>()
                                        )
                                    },
                                    client
                                ) {
                                    println!("TrkCom::ADD send_to err");
                                } else {
                                    println!("TrkCom::ADD send_to ok");
                                }
                            }
                            TrkCom::REMOVE => {
                                if let Some(pos) = clients.iter().position(|&addr| addr == client) {
                                    clients.remove(pos);
                                    if let Err(_) = tracker.send_to(
                                        unsafe {
                                            std::slice::from_raw_parts(
                                                &com as *const _ as *mut u8, std::mem::size_of::<TrkCom>()
                                            )
                                        },
                                        client
                                    ) {
                                        println!("TrkCom::REMOVE send_to err");
                                    } else {
                                        println!("TrkCom::REMOVE send_to ok");
                                    }
                                }
                            }
                        }
                    }
                    if clients.len() != 0 {
                        i = (i + 1) % clients.len();
                    }
                }
            }
        }
    }
}

fn main() {
    if let Ok(main_sock) = std::net::UdpSocket::bind((std::net::Ipv4Addr::from(0), 16673)) {
        let mut trackers = std::vec::Vec::<std::thread::JoinHandle<_>>::new();
        loop {
            if let Ok((_, std::net::SocketAddr::V4(client))) = main_sock.recv_from(&mut [0; 0]) {
                trackers.push(std::thread::spawn(move || p2p_tracker(client)));
                println!("new");
            }
        }
    } else {
        println!("shit");
    }
}
