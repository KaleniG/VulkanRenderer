namespace vkren
{

  template<typename T>
  Ref<VertexBuffer> VertexBuffer::Create(const std::vector<T>& vertices)
  {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    StagingBuffer stagingBuffer = StagingBuffer::Create(bufferSize);
    stagingBuffer.Map();
    stagingBuffer.Update((void*)vertices.data());
    stagingBuffer.Unmap();

    Ref<VertexBuffer> buffer = VertexBuffer::Create(bufferSize);
    stagingBuffer.CopyToBuffer(*buffer.get());

    return buffer;
  }


}