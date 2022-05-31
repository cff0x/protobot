create table bot_options
(
    option varchar not null
        constraint bot_options_pk
            primary key,
    value  varchar
);

alter table bot_options
    owner to protobot;

create unique index bot_options_option_uindex
    on bot_options (option);

