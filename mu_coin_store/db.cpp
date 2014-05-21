#include <mu_coin_store/db.hpp>
#include <boost/filesystem.hpp>

mu_coin_store::block_store_db_temp_t mu_coin_store::block_store_db_temp;

mu_coin_store::block_store_db::block_store_db (block_store_db_temp_t const &) :
handle (nullptr, 0)
{
    boost::filesystem::path temp (boost::filesystem::unique_path ());
    handle.open (nullptr, temp.native().c_str (), nullptr, DB_HASH, DB_CREATE | DB_EXCL, 0);
}

std::unique_ptr <mu_coin::transaction_block> mu_coin_store::block_store_db::latest (mu_coin::address const & address_a)
{
    std::unique_ptr <mu_coin::transaction_block> result;
    bool exists;
    uint16_t sequence;
    latest_sequence (address_a, sequence, exists);
    if (exists)
    {
        mu_coin::block_id block (address_a, sequence);
        dbt key (block);
        dbt value;
        int error (handle.get (nullptr, &key.data, &value.data, 0));
        result = value.block ();
    }
    return result;
}

void mu_coin_store::block_store_db::insert_block (mu_coin::block_id const & id_a, mu_coin::transaction_block const & block_a)
{
    dbt key (id_a);
    dbt data (block_a);
    int error (handle.put (nullptr, &key.data, &data.data, 0));
    dbt key2 (id_a.address);
    dbt data2 (id_a.sequence);
    int error2 (handle.put (nullptr, &key2.data, &data2.data, 0));
}

void mu_coin_store::block_store_db::latest_sequence (mu_coin::address const & address_a, uint16_t & sequence, bool & exists)
{
    dbt key (address_a);
    dbt value;
    int error (handle.get (nullptr, &key.data, &value.data, 0));
    if (value.data.get_size () == 2)
    {
        exists = true;
        mu_coin::byte_read_stream stream (reinterpret_cast <uint8_t *> (value.data.get_data ()), reinterpret_cast <uint8_t *> (value.data.get_data ()) + value.data.get_size ());
        stream.read (sequence);
    }
    else
    {
        exists = false;
    }
}

std::unique_ptr <mu_coin::transaction_block> mu_coin_store::block_store_db::block (mu_coin::block_id const & id_a)
{
    mu_coin_store::dbt key (id_a);
    mu_coin_store::dbt data;
    int error (handle.get (nullptr, &key.data, &data.data, 0));
    auto result (data.block ());
    return result;
}

std::unique_ptr <mu_coin::transaction_block> mu_coin_store::dbt::block()
{
    std::unique_ptr <mu_coin::transaction_block> result;
    if (data.get_size () > 0)
    {
        auto item (std::unique_ptr <mu_coin::transaction_block> (new mu_coin::transaction_block));
        mu_coin::byte_read_stream stream (reinterpret_cast <uint8_t *> (data.get_data ()), reinterpret_cast <uint8_t *> (data.get_data ()) + data.get_size ());
        auto error (item->deserialize (stream));
        if (!error)
        {
            result = std::move (item);
        }
    }
    return result;
}

mu_coin_store::dbt::dbt (mu_coin::transaction_block const & block_a)
{
    mu_coin::byte_write_stream stream;
    block_a.serialize (stream);
    adopt (stream);
}

mu_coin_store::dbt::dbt (mu_coin::address const & address_a)
{
    mu_coin::byte_write_stream stream;
    address_a.serialize (stream);
    adopt (stream);
}

mu_coin_store::dbt::dbt (mu_coin::block_id const & id_a)
{
    mu_coin::byte_write_stream stream;
    id_a.serialize (stream);
    adopt (stream);
}

mu_coin_store::dbt::dbt (uint16_t sequence_a)
{
    mu_coin::byte_write_stream stream;
    stream.write (sequence_a);
    adopt (stream);
}

void mu_coin_store::dbt::adopt (mu_coin::byte_write_stream & stream_a)
{
    data.set_data (stream_a.data);
    data.set_size (stream_a.size);
    stream_a.data = nullptr;
}

std::unique_ptr <mu_coin::send_block> mu_coin_store::block_store_db::send (mu_coin::address const & address_a, mu_coin::block_id const & id_a)
{
    mu_coin_store::dbt key (id_a);
    mu_coin_store::dbt data;
    int error (handle.get (nullptr, &key.data, &data.data, 0));
    auto result (error != 0);
    assert (false);
    return nullptr;
}

void mu_coin_store::block_store_db::insert_send (mu_coin::address const & address_a, mu_coin::send_block const & block_a)
{
    assert (false);
}

void mu_coin_store::block_store_db::clear (mu_coin::address const & address_a, mu_coin::block_id const & id_a)
{
    assert (false);
}