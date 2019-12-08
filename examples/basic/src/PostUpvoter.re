open GraphqlHooksMutation;
open Util;

module UpdatePostConfig = [%graphql
  {|
    mutation updatePost($id: ID!, $votes: Int) {
      updatePost(id: $id, votes: $votes) {
        id
        __typename
        votes
      }
    }
  |}
];

[@react.component]
let make = (~id: string, ~votes: int, ~onUpdate: unit => unit) => {
  let request =
    UpdatePostConfig.make(
      ~id,
      ~votes={
        votes + 1;
      },
      (),
    );

  let (_, updatePost) = useMutation(~request, ());

  <button
    onClick={_ =>
      updatePost()
      |> Js.Promise.then_(_result => onUpdate() |> Js.Promise.resolve)
      |> ignore
    }>
    {votes->string_of_int->s}
  </button>;
};